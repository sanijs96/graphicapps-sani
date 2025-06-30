#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_app_cmd.h"

#include "vulkan_obj_mgr/vulkan_obj_mgr.h"
#include "vulkan_obj_mgr/vulkan_function.h"
#include "vulkan_obj_mgr/device/vulkan_device.h"
#include "vulkan_obj_mgr/instance/vulkan_instance.h"

#include "vulkan_ops_mgr/vulkan_ops_mgr.h"
#include "vulkan_ops_mgr/pipeline/vulkan_pipeline.h"

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

    struct {
        uint32_t stage_idx;
        char filename[FILENAME_MAX];
    } pipeline;

} vulkan_cmd_args_list_t;

static uint32_t __setup_layer_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(LAYER_NAME):
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
        case PARAM_VK(EXTENSION_NAME):
            strcpy(p_arglist->extension.name, arg.value);
            break;

        case PARAM_VK(EXTENSION_SCOPE):
            p_arglist->extension.scope = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(EXTENSION_PHYDEV_IDX):
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
        case PARAM_VK(DEVICE_PHYDEV_IDX):
            p_arglist->device.phydev_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(DEVICE_SHOW_EXTENSION):
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

static uint32_t __setup_pipeline_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(PIPELINE_STAGE_IDX):
            p_arglist->pipeline.stage_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(PIPELINE_SHADER_FILENAME):
            strcpy(p_arglist->pipeline.filename, arg.value);
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t _vulkan_cmd_check_arg_exist(command_t *p_cmd, uint32_t arg_type)
{
    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        if (p_cmd->p_args[idx].type == arg_type) {
            return SUCCESS;
        }
    }

    return FAILURE;
}

static uint32_t _vulkan_cmd_setup_argument_list(command_t *p_cmd,
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
    else if (!strcmp(p_cmd->cmd_name, "pipeline")) {
        __arg_setup_func = __setup_pipeline_argument;
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

uint32_t _vulkan_cmd_enable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_enable_layer(args_list.layer.name);
}

uint32_t _vulkan_cmd_disable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_disable_layer(args_list.layer.name);
}

uint32_t _vulkan_cmd_show_layers_list(command_t *p_cmd)
{
    vulkan_obj_mgr_show_layers_list();

    return  SUCCESS;
}

uint32_t _vulkan_cmd_enable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        return vulkan_obj_mgr_enable_extension(args_list.extension.name);
    }
    else if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        if (_vulkan_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FAILURE) {
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

uint32_t _vulkan_cmd_disable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        return vulkan_obj_mgr_enable_extension(args_list.extension.name);
    }
    else if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        if (_vulkan_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FAILURE) {
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

uint32_t _vulkan_cmd_show_extensions_list(command_t *p_cmd)
{
    vulkan_obj_mgr_show_extensions_list();

    return SUCCESS;
}

uint32_t _vulkan_cmd_create_instance(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_create_instance();
}


uint32_t _vulkan_cmd_create_device(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (_vulkan_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_obj_mgr_create_device(args_list.device.phydev_idx) == FAILURE) {
        return FAILURE;
    }

    VkDevice *p_device;
    VkPhysicalDevice *p_phydev;

    p_device = vulkan_obj_mgr_get_current_device_object();
    p_phydev = vulkan_obj_mgr_get_phydev_object(args_list.device.phydev_idx);

    window_obj_mgr_setup_device_ctx(p_phydev, p_device);
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

void _vulkan_cmd_show_display_support(uint32_t phydev_idx)
{
    VkSurfaceKHR *p_display_object;

    p_display_object = window_obj_mgr_get_current_display_object();

    __check_presentation_support(phydev_idx, p_display_object);
}

uint32_t _vulkan_cmd_show_devices_list(command_t *p_cmd)
{
    uint32_t phydev_idx;
    uint32_t phydev_count;
    vulkan_cmd_args_list_t args_list;

    phydev_count = device_get_phydevs_count();

    if (instance_check_creation_state() != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        printf("instance not created yet\n");
        return FAILURE;
    }

    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    printf("[DEVICE LIST]\n");
    if (_vulkan_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FAILURE) {
        phydev_idx = 0;
    }
    else {
        phydev_idx = args_list.device.phydev_idx;

        if (phydev_idx >= phydev_count) {
            printf("invalid device index\n");
            return FAILURE;
        }

        phydev_count = args_list.device.phydev_idx + 1;
    }

    for (; phydev_idx < phydev_count; phydev_idx++) {
        vulkan_obj_mgr_show_device_info(phydev_idx);

        if (args_list.device.show_extension == TRUE) {
            vulkan_obj_mgr_show_device_extensions_list(phydev_idx);
        }

        if (window_obj_mgr_check_display_status() == WINDOW_OBJ_DISPLAY_STATE_CREATED) {
            _vulkan_cmd_show_display_support(phydev_idx);
        }
    }

    return SUCCESS;
}

uint32_t _vulkan_cmd_setup_pipeline_stage(command_t *p_cmd)
{
    uint32_t res;
    uint32_t pipeline_stage_idx;

    VkDevice *p_device;
    vulkan_cmd_args_list_t args_list;

    if (_vulkan_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    pipeline_stage_idx = args_list.pipeline.stage_idx;

    switch (pipeline_stage_idx) {
        case VULKAN_PIPELINE_STAGE_VERTEX_SHADER:
        case VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER:
            p_device = vulkan_obj_mgr_get_current_device_object();
            res = vulkan_ops_mgr_add_shader_file(pipeline_stage_idx,
                                                    args_list.pipeline.filename, p_device);
            break;

        default:
            printf("changing stage of %u is not allowed(yet)\n", pipeline_stage_idx);
            return FAILURE;

    }

    return res;
}

uint32_t _vulkan_cmd_run_pipeline(command_t *p_cmd)
{
    uint32_t res;
    VkDevice *p_device;

    p_device = vulkan_obj_mgr_get_current_device_object();
    if (p_device == NULL) {
        printf("device is not created\n");

        return FAILURE;
    }

    res = vulkan_ops_mgr_create_pipeline(p_device);

    return res;
}

uint32_t _vulkan_cmd_show_pipeline_info(command_t *p_cmd)
{

}

