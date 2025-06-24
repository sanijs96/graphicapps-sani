#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_obj_mgr_cmd.h"

#include "vulkan_obj_mgr/vulkan_obj_mgr.h"
#include "vulkan_obj_mgr/vulkan_function.h"
#include "vulkan_obj_mgr/device/vulkan_device.h"
#include "vulkan_obj_mgr/instance/vulkan_instance.h"

#include "window_obj_mgr/window_obj_mgr.h"

typedef union vulkan_cmd_args_list {
    struct {
        uint32_t scope;
        char name[VK_MAX_EXTENSION_NAME_SIZE];
    } layer;

    struct {
        uint32_t scope;
        uint32_t phydev_idx;
        char name[VK_MAX_EXTENSION_NAME_SIZE];
    } extension;

    struct {
        uint32_t phydev_idx;
        uint32_t show_extension;
    } device;
} vulkan_cmd_args_list_t;

static uint32_t __setup_layer_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case APPS_PARAM_TYPE_VULKAN_LAYER_NAME:
            strcpy(p_arglist->layer.name, arg.value);
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

        case APPS_PARAM_TYPE_VULKAN_EXTENSION_SCOPE:
            p_arglist->extension.scope = (uint32_t)(*(char *)arg.value - '0');
            break;

        case APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX:
            p_arglist->extension.phydev_idx = (uint32_t)(*(char *)arg.value - '0');
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

        case APPS_PARAM_TYPE_VULKAN_DEVICE_SHOW_EXTENSION:
            p_arglist->device.show_extension = ((uint32_t)(*(char *)arg.value - '0') == 1) ?
                                                                                TRUE : FALSE;
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_instance_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    return SUCCESS;
}

static uint32_t _vulkan_obj_mgr_cmd_check_arg_exist(command_t *p_cmd, uint32_t arg_type)
{
    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        if (p_cmd->p_args[idx].type == arg_type) {
            return SUCCESS;
        }
    }

    return FAILURE;
}

static uint32_t _vulkan_obj_mgr_cmd_setup_argument_list(command_t *p_cmd,
                                        vulkan_cmd_args_list_t *p_arglist)
{
    uint32_t res;
    uint32_t (*__arg_setup_func)(command_arg_t, vulkan_cmd_args_list_t *);

    if (p_cmd->num_args == 0) {
        return SUCCESS;
    }

    if (!strcmp(p_cmd->cmd_name, "layer")) {
        __arg_setup_func = __setup_layer_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "extension")) {
        __arg_setup_func = __setup_extension_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "device")) {
        __arg_setup_func = __setup_device_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "instance")) {
        __arg_setup_func = __setup_instance_argument;
    }
    else {
        return FAILURE;
    }

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        res = __arg_setup_func(p_cmd->p_args[idx], p_arglist);
        if (res == FAILURE) {
            break;
        }
    }

    return res;
}

uint32_t _vulkan_obj_mgr_cmd_enable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_enable_layer(args_list.layer.name);
}

uint32_t _vulkan_obj_mgr_cmd_disable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_disable_layer(args_list.layer.name);
}

uint32_t _vulkan_obj_mgr_cmd_show_layers_list(command_t *p_cmd)
{
    vulkan_obj_mgr_show_layers_list();

    return  SUCCESS;
}

uint32_t _vulkan_obj_mgr_cmd_enable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        return vulkan_obj_mgr_enable_extension(args_list.extension.name);
    }
    else if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        if (_vulkan_obj_mgr_cmd_check_arg_exist(p_cmd,
                                        APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX) == FAILURE) {
            printf("no physical device selected\n");

            return FAILURE;
        }

        return vulkan_obj_mgr_enable_phydev_extension(args_list.extension.name,
                                                        args_list.extension.phydev_idx);
    }
    else {
        return FAILURE;
    }
}

uint32_t _vulkan_obj_mgr_cmd_disable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        return vulkan_obj_mgr_enable_extension(args_list.extension.name);
    }
    else if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        if (_vulkan_obj_mgr_cmd_check_arg_exist(p_cmd,
                                        APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX) == FAILURE) {
            printf("no physical device selected\n");

            return FAILURE;
        }

        return vulkan_obj_mgr_disable_phydev_extension(args_list.extension.name,
                                                        args_list.extension.phydev_idx);
    }
    else {
        return FAILURE;
    }
}

uint32_t _vulkan_obj_mgr_cmd_show_extensions_list(command_t *p_cmd)
{
    vulkan_obj_mgr_show_extensions_list();

    return SUCCESS;
}

uint32_t _vulkan_obj_mgr_cmd_create_instance(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_create_instance();
}


uint32_t _vulkan_obj_mgr_cmd_create_device(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (_vulkan_obj_mgr_cmd_check_arg_exist(p_cmd, APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX)) {
        return FAILURE;
    }

    return vulkan_obj_mgr_create_device(args_list.device.phydev_idx);
}

static void __check_presentation_support(uint32_t phydev_idx, VkSurfaceKHR *p_surface)
{
    uint32_t queue_count;
    uint32_t present_support;
    VkPhysicalDevice *p_phydev;

    p_phydev = device_get_phydev_object(phydev_idx);

    queue_count = device_get_device_queue_property_count(phydev_idx);

    printf("Presentation Queue Index: ");
    for (uint32_t queue_idx = 0; queue_idx < queue_count; queue_idx++)  {
        present_support = FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(*p_phydev, queue_idx, *p_surface, &present_support);

        if (present_support == TRUE) {
            printf("%u ", queue_idx);
        }
    }
    printf("\n");

    return;
}

void _vulkan_obj_mgr_cmd_show_display_support(uint32_t phydev_idx)
{
    VkSurfaceKHR *p_display_object;

    p_display_object = window_obj_mgr_get_display_object();

    __check_presentation_support(phydev_idx, p_display_object);
}

uint32_t _vulkan_obj_mgr_cmd_show_devices_list(command_t *p_cmd)
{
    uint32_t phydev_idx;
    uint32_t phydev_count;
    vulkan_cmd_args_list_t args_list;

    phydev_count = device_get_phydevs_count();

    if (instance_check_creation_state() != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        printf("instance not created yet\n");
        return FAILURE;
    }

    if (_vulkan_obj_mgr_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (_vulkan_obj_mgr_cmd_check_arg_exist(p_cmd, APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX)) {
        phydev_idx = args_list.device.phydev_idx;

    }
    else {
        phydev_idx = 0;
    }

    if (phydev_idx >= phydev_count) {
        printf("invalid device index\n");
        return FAILURE;
    }

    printf("[DEVICE LIST]\n");
    for (; phydev_idx < phydev_count; phydev_idx++) {
        vulkan_obj_mgr_show_device_info(phydev_idx);

        if (args_list.device.show_extension == TRUE) {
            vulkan_obj_mgr_show_device_extensions_list(phydev_idx);
        }

        if (window_obj_mgr_check_display_status() == WINDOW_DISPLAY_SURFACE_STATE_CREATED) {
            _vulkan_obj_mgr_cmd_show_display_support(phydev_idx);
        }
    }

    return SUCCESS;
}
