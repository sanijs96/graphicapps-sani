#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "app_cmd.h"

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

uint32_t app_cmd_get_cmdstring_from_runscript(char *p_input_str)
{
    uint64_t cmd_strlen;
    uint32_t runscript_offset;
    char *p_runscript_data;

get_cmd:
    runscript_offset = app_cmd_ctx.runscript_offset;

    p_runscript_data = &app_cmd_ctx.p_runscript_data[runscript_offset];

    cmd_strlen = (uint64_t)(strstr(p_runscript_data, "\n\0") - p_runscript_data);
    if (cmd_strlen != 0) {
        cmd_strlen++; // include eol

        strncpy(p_input_str, p_runscript_data, cmd_strlen);
        printf("[%u] %s\n", app_cmd_ctx.runscript_cmd_seq, p_input_str);

        app_cmd_ctx.runscript_offset += cmd_strlen;
        app_cmd_ctx.runscript_cmd_seq++;
    }
    else {
        app_cmd_ctx.runscript_offset++;
        if (app_cmd_ctx.runscript_offset < app_cmd_ctx.runscript_total_length) {
            goto get_cmd; // skip single eol character
        }
    }

    if (app_cmd_ctx.runscript_offset == app_cmd_ctx.runscript_total_length) {
        __app_cmd_cleanup_runscript_ctx();
    }

    return SUCCESS;
}

uint32_t app_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
