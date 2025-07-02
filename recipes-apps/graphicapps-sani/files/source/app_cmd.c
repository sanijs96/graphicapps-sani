#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "app_cmd.h"

enum app_cmd_input_src_type {
    APP_CMD_INPUT_SRC_TYPE_STDIN = 0,
    APP_CMD_INPUT_SRC_TYPE_SCRIPTFILE,
    NUM_APP_CMD_INPUT_SRC_TYPES,
};

static struct {
    uint32_t input_src_type;

    uint32_t inputfile_offset;
    char *p_inputfile_data;

    char p_input_entry_str[MAX_LENGTH_APP_CMD];
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
    char *p_inputfile_data;

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

    app_cmd_ctx.p_inputfile_data = (char *)malloc(file_len);

    if (fread(app_cmd_ctx.p_inputfile_data, file_len, 1, p_fstream) == 0) {
        printf("file read failure\n");
        return FAILURE;
    }

    app_cmd_ctx.input_src_type = APP_CMD_INPUT_SRC_TYPE_SCRIPTFILE;
}

static void __app_cmd_cleanup_runscript_ctx(void)
{
    app_cmd_ctx.input_src_type = APP_CMD_INPUT_SRC_TYPE_STDIN;
    app_cmd_ctx.inputfile_offset = 0;

    free(app_cmd_ctx.p_inputfile_data);
}

static void __app_cmd_get_cmd_entry_from_runscript(char *p_input_str)
{
    uint64_t cmd_strlen;
    uint32_t inputfile_offset;
    char **pp_inputfile_data;

    inputfile_offset = app_cmd_ctx.inputfile_offset;

    *pp_inputfile_data = &app_cmd_ctx.p_inputfile_data[inputfile_offset];

    cmd_strlen = (uint64_t)(strstr(*pp_inputfile_data, "\n\0") - *pp_inputfile_data);
    if (cmd_strlen != 0) {
        printf("cmdlen: %lu, %s\n", cmd_strlen, *pp_inputfile_data);
        p_input_str = strndup(*pp_inputfile_data, cmd_strlen);

        app_cmd_ctx.inputfile_offset += cmd_strlen;
    }
    else {
        printf("parsing failure\n");
        p_input_str = NULL;
    }

    // check EOF
    if (app_cmd_ctx.p_inputfile_data[app_cmd_ctx.inputfile_offset] == '\0') {
        __app_cmd_cleanup_runscript_ctx();
    }

    return;
}

char *app_cmd_get_cmdstring(void)
{
    char *p_input_str;

    p_input_str = app_cmd_ctx.p_input_entry_str;

    memset(p_input_str, 0, sizeof(MAX_LENGTH_APP_CMD));

    if (app_cmd_ctx.input_src_type == APP_CMD_INPUT_SRC_TYPE_SCRIPTFILE) {
        __app_cmd_get_cmd_entry_from_runscript(p_input_str);
    }
    else if (!fgets(p_input_str, MAX_LENGTH_APP_CMD, stdin)) {
        p_input_str = NULL;
    }

    return p_input_str;
}

