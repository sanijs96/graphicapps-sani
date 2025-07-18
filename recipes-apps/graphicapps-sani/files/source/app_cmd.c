#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "app_cmd.h"

#include "app_utilities/datascript_parser.h"

#define MAX_NUM_REGVAL_ENTRIES  (20)
#define MAX_REGVAL_NAME_LENGTH  (20)
#define MAX_REGVAL_LENGTH       (5)

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

uint32_t app_cmd_add_runscript_file(command_t *p_cmd)
{
    app_cmd_args_list_t args_list;

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        if (__setup_app_argument(p_cmd->p_args[idx], &args_list) == FAILURE) {
            return FAILURE;
        }
    }

    if (datascript_register_scriptfile(args_list.scriptfile.name) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t app_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
