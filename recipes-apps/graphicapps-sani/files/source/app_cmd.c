#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "app_cmd.h"

#define MAX_NUM_REGVAL_ENTRIES  (20)
#define MAX_REGVAL_NAME_LENGTH  (20)
#define MAX_REGVAL_LENGTH       (5)

static struct {
    uint32_t runscript_state;

    uint32_t runscript_offset;
    uint32_t runscript_cmd_seq;
    uint32_t runscript_total_length;
    char *p_runscript_data;
} app_cmd_ctx;

typedef union app_cmd_args_list {
    struct {
        char name[FILENAME_MAX];
    } scriptfile;
} app_cmd_args_list_t;

typedef struct regval_entry {
    char name[MAX_REGVAL_NAME_LENGTH];
    char value[MAX_REGVAL_LENGTH];
} regval_entry_t;

static struct {
    uint32_t count;
    regval_entry_t regvals[MAX_NUM_REGVAL_ENTRIES];
} saved_args_ctx;

static uint32_t __setup_app_argument(command_arg_t arg, app_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_APP(SCRIPTFILE_NAME):
            strcpy(p_arglist->scriptfile.name, arg.value);
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __app_cmd_get_file_length(FILE *p_fstream)
{
    uint32_t file_len;
    fpos_t pos;

    // TODO: divide with units of size_t(=SIZE_MAX)
    if (fseek(p_fstream, 0, SEEK_END) != 0) {
        return 0;
    }
    if (fgetpos(p_fstream, &pos) != 0) {
        return 0;
    }

    file_len = pos.__pos;

    pos.__pos = 0;
    if (fsetpos(p_fstream, &pos) != 0) {
        return 0;
    }

    return file_len;
}
uint32_t app_cmd_add_runscript_file(command_t *p_cmd)
{
    uint32_t file_len;
    FILE * p_fstream;
    app_cmd_args_list_t args_list;
    char *p_runscript_data;

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        if (__setup_app_argument(p_cmd->p_args[idx], &args_list) == FAILURE) {
            return FAILURE;
        }
    }

    p_fstream = fopen(args_list.scriptfile.name, "r");
    if (p_fstream == NULL) {
        printf("check filename: %s\n", args_list.scriptfile.name);
        return FAILURE;
    }

    file_len = __app_cmd_get_file_length(p_fstream);
    if (file_len == 0) {
        printf("file length is invalid(%u)\n", file_len);
        return FAILURE;
    }

    app_cmd_ctx.p_runscript_data = (char *)malloc(file_len);

    if (fread(app_cmd_ctx.p_runscript_data, file_len, 1, p_fstream) == 0) {
        printf("file read failure\n");
        return FAILURE;
    }

    app_cmd_ctx.runscript_offset = 0;
    app_cmd_ctx.runscript_cmd_seq = 0;
    app_cmd_ctx.runscript_total_length = file_len;

    app_cmd_ctx.runscript_state = APP_CMD_RUNSCRIPT_REGISTERED;
}

static void __app_cmd_cleanup_runscript_ctx(void)
{
    app_cmd_ctx.runscript_offset = 0;
    app_cmd_ctx.runscript_cmd_seq = 0;
    app_cmd_ctx.runscript_total_length = 0;

    free(app_cmd_ctx.p_runscript_data);

    app_cmd_ctx.runscript_state = APP_CMD_RUNSCRIPT_FINISHED;
}

uint32_t app_cmd_get_runscript_state(void)
{
    return app_cmd_ctx.runscript_state;
}

static uint32_t __app_cmd_check_comment_string(char *p_input_str)
{
    if (p_input_str[0] == '#') {
        return TRUE;
    }

    return FALSE;
}

uint32_t app_cmd_get_cmdstring_from_runscript(char *input_str)
{
    uint64_t cmd_strlen;
    uint32_t runscript_offset;
    char *p_runscript_data;

get_new_cmd:
    runscript_offset = app_cmd_ctx.runscript_offset;

    p_runscript_data = &app_cmd_ctx.p_runscript_data[runscript_offset];

    cmd_strlen = (uint64_t)(strstr(p_runscript_data, "\n\0") - p_runscript_data);
    if (cmd_strlen != 0) {
        cmd_strlen++; // include eol

        strncpy(input_str, p_runscript_data, cmd_strlen);

        app_cmd_ctx.runscript_offset += cmd_strlen;

        if (__app_cmd_check_comment_string(input_str) == TRUE) {
            memset(input_str, 0, cmd_strlen);

            goto get_new_cmd;
        }

        printf("[%u] %s\n", app_cmd_ctx.runscript_cmd_seq, input_str);

        app_cmd_ctx.runscript_cmd_seq++;
    }
    else {
        app_cmd_ctx.runscript_offset++;
        if (app_cmd_ctx.runscript_offset < app_cmd_ctx.runscript_total_length) {
            goto get_new_cmd; // skip single eol character
        }
    }

    if (app_cmd_ctx.runscript_offset == app_cmd_ctx.runscript_total_length) {
        __app_cmd_cleanup_runscript_ctx();
    }

    return SUCCESS;
}

static uint32_t __app_cmd_get_regval_entry_idx(char *input_str)
{
    regval_entry_t *p_regval_entry;

    p_regval_entry = &saved_args_ctx.regvals[0];

    for (uint32_t idx = 0; idx < saved_args_ctx.count; idx++) {
        if (strncmp(p_regval_entry[idx].name, input_str, strlen(p_regval_entry[idx].name))) {
            continue;
        }

        return idx;
    }

    return MAX_NUM_REGVAL_ENTRIES;
}

uint32_t app_cmd_check_argname_registered(char *input_str)
{
    if (input_str[0] != '$') {
        return FALSE;
    }

    if (__app_cmd_get_regval_entry_idx(input_str) == MAX_NUM_REGVAL_ENTRIES) {
        return FALSE; // no matching entry
    }

    return TRUE;
}

char *app_cmd_load_argval_from_argname(char *arg_name)
{
    uint32_t regval_entry_idx;
    char *regval_str;

    regval_entry_idx = __app_cmd_get_regval_entry_idx(arg_name);

    if (regval_entry_idx == MAX_NUM_REGVAL_ENTRIES) {
        printf("argname %s not registered\n", arg_name);

        return NULL;
    }

    return saved_args_ctx.regvals[regval_entry_idx].value;
}

uint32_t app_cmd_save_result(char *arg_name, uint32_t retval)
{
    uint32_t regval_entry_idx;
    char *retval_str;
    if (app_cmd_check_argname_registered(arg_name) == TRUE) {
        regval_entry_idx = __app_cmd_get_regval_entry_idx(arg_name);
    }
    else {
        regval_entry_idx = saved_args_ctx.count;

        if (regval_entry_idx == MAX_NUM_REGVAL_ENTRIES) {
            printf("too many arguments registered\n");
            return FAILURE;
        }
    }

    saved_args_ctx.regvals[regval_entry_idx].name[0] = '$';

    strncpy(&saved_args_ctx.regvals[regval_entry_idx].name[1], arg_name, strlen(arg_name));

    sprintf(saved_args_ctx.regvals[regval_entry_idx].value, "%u", retval);

    saved_args_ctx.count++;

    return SUCCESS;
}

uint32_t app_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
