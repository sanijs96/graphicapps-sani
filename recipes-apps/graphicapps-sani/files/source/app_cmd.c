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

uint32_t __app_cmd_setup_resource_info_ctx(resource_info_t *p_info)
{
    p_info->count = datascript_get_resource_member_count(p_info->name);
    if (p_info->count == 0) {
        return FAILURE;
    }

    p_info->type = datascript_get_resource_type(p_info->name);
    if (p_info->type == RESOURCE_FORMAT_TYPE_INVALID) {
        return FAILURE;
    }

    if (p_info->type >= RESOURCE_FORMAT_TYPE_IMAGE_START) {
        // TBD
    }
    else if (p_info->type >= RESOURCE_FORMAT_TYPE_INDEX_BUFFER_START) {
        resource_description_t description;

        if (vulkan_resource_mgr_get_resource_description(p_info, &description) == FAILURE) {
            return FAILURE;
        }

        p_info->index_buffer.index_type = description.index_buffer.index_type;
    }
    else {
        p_info->vertex_buffer.binding_idx = datascript_get_resource_entry_idx(p_info->name);
        if (p_info->vertex_buffer.binding_idx == MAX_NUM_RESOURCE_OBJECTS) {
            return FAILURE;
        }
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

    if (datascript_load_all_resource_objects() == FAILURE) {
        return FAILURE;
    }

    num_resources = datascript_get_total_resource_count();
    if (num_resources == 0) {
        printf("no resource dectected\n");
        return FAILURE;
    }

    resource_info_t info;

    for (uint32_t idx = 0; idx < num_resources; idx++) {
        memset(&info, 0, sizeof(resource_info_t));
        if (datascript_get_resource_name(idx, info.name) == FAILURE) {
            return FAILURE;
        }

        if (__app_cmd_setup_resource_info_ctx(&info) == FAILURE) {
            return FAILURE;
        }

        if (vulkan_resource_mgr_add_resource_info(&info) == FAILURE) {
            return FAILURE;
        }
    }

    return SUCCESS;
}

uint32_t app_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
