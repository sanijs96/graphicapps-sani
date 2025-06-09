#include <string.h>

#include "common/common_def.h"

#include "vulkan_obj_mgr.h"
#include "vulkan_obj_mgr_cmd.h"

typedef union vulkan_cmd_args_list {
    struct {
        uint32_t scope;
        char name[VK_MAX_EXTENSION_NAME_SIZE];
    } layer;

    struct {
        uint32_t scope;
        char name[VK_MAX_EXTENSION_NAME_SIZE];
    } extension;

    struct {
        uint32_t phydev_idx;
    } device;
} vulkan_cmd_args_list_t;

static uint32_t __setup_layer_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case APPS_PARAM_TYPE_VULKAN_LAYER_NAME:
            strcpy(p_arglist->layer.name, arg.value);
            break;

        case APPS_PARAM_TYPE_VULKAN_LAYER_SCOPE:
            p_arglist->layer.scope = (uint32_t)(*(char *)arg.value - '0');
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_extension_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case APPS_PARAM_TYPE_VULKAN_EXTENSION_NAME:
            strcpy(p_arglist->extension.name, arg.value);
            break;

        case APPS_PARAM_TYPE_VULKAN_LAYER_SCOPE:
            p_arglist->extension.scope = (uint32_t)(*(char *)arg.value - '0');
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_device_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case APPS_PARAM_TYPE_VULKAN_DEVICE_PHYDEV_IDX:
            p_arglist->device.phydev_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t _vulkan_obj_mgr_cmd_setup_argument_list(command_t *p_cmd,
                                        vulkan_cmd_args_list_t *p_arglist)
{
    uint32_t res;
    command_arg_t argument;

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        argument = p_cmd->p_args[idx];

        switch (p_cmd->opcode) {
            case OPCODE_VK(LAYER, ADD):
            case OPCODE_VK(LAYER, DEL):
            case OPCODE_VK(LAYER, SHOW_LIST):
                res = __setup_layer_argument(argument, p_arglist);
                break;

            case OPCODE_VK(EXTENSION, ADD):
            case OPCODE_VK(EXTENSION, DEL):
            case OPCODE_VK(EXTENSION, SHOW_LIST):
                res = __setup_extension_argument(argument, p_arglist);
                break;

            case OPCODE_VK(DEVICE, CREATE):
            case OPCODE_VK(DEVICE, SHOW_LIST):
                res = __setup_device_argument(argument, p_arglist);
                break;

            case OPCODE_VK(INSTANCE, CREATE):

            default:
                return FAILURE;
        }

        if (res == FAILURE) {
            break;
        }
    }

    return res;
}

uint32_t _vulkan_obj_mgr_enable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_enable_layer(args_list.layer.name, args_list.layer.scope);
}

uint32_t _vulkan_obj_mgr_disable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_disable_layer(args_list.layer.name, args_list.layer.scope);
}

uint32_t _vulkan_obj_mgr_enable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_enable_extension(args_list.extension.name, args_list.extension.scope);
}

uint32_t _vulkan_obj_mgr_disable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_disable_extension(args_list.extension.name, args_list.extension.scope);
}

uint32_t _vulkan_obj_mgr_create_instance(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_create_instance();
}

uint32_t _vulkan_obj_mgr_create_device(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_create_device(args_list.device.phydev_idx);
}
