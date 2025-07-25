#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "app_utilities/datascript_parser.h"

#include "vulkan_resource/vulkan_resource_mgr.h"

#include "app_cmd.h"

typedef union app_cmd_args_list {
    struct {
        char name[FILENAME_MAX];
    } datascript;
} app_cmd_args_list_t;

static uint32_t __setup_app_argument(command_arg_t arg, app_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_APP(DATASCRIPT_NAME):
            strcpy(p_arglist->datascript.name, arg.value);
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

    if (datascript_register_scriptfile(args_list.datascript.name) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t app_cmd_load_resource_data(command_t *p_cmd)
{
    uint32_t num_resources;
    app_cmd_args_list_t args_list;

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        if (__setup_app_argument(p_cmd->p_args[idx], &args_list) == FAILURE) {
            return FAILURE;
        }
    }

    if (datascript_register_datafile(args_list.datascript.name) == FAILURE) {
        return FAILURE;
    }

    num_resources = datascript_get_resource_count();
    if (num_resources == 0) {
        printf("no resource dectected\n");
        return FAILURE;
    }

    resource_info_t info_list[num_resources];
    memset(info_list, 0, sizeof(resource_info_t) * num_resources);

    if (datascript_load_resource_info_from_datafile(info_list) == FAILURE) {
        return FAILURE;
    }

    for (uint32_t idx = 0; idx < num_resources; idx++) {
        if (vulkan_resource_mgr_add_resource_info(&info_list[idx]) == FAILURE) {
            return FAILURE;
        }
    }

    return SUCCESS;
}

uint32_t app_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
