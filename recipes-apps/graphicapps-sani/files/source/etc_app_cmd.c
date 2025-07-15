#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"
#include "vulkan/cmd_types.h"

#include "etc_app_cmd.h"

#include "window_obj_mgr/window_obj_mgr.h"
#include "vulkan_obj_mgr/vulkan_obj_mgr.h"
#include "vulkan_ops_mgr/vulkan_ops_mgr.h"

typedef union etc_cmd_args_list {
    struct {
        VkInstance* p_instance;
    } window_display;

    struct {
        uint32_t width;
        uint32_t height;
    } window_resize;

} etc_cmd_args_list_t;

static uint32_t __parse_window_size_value(command_arg_t arg)
{
    uint32_t argval_int;
    char *p_argval_digit;

    if ((arg.type != PARAM_ETC(WINDOW_WIDTH)) &&
        (arg.type != PARAM_ETC(WINDOW_HEIGHT))) {
        return 0;
    }

    if (strlen(arg.value) > 3) { // maximum size < 1000
        return 0;
    }

    argval_int = 0;
    p_argval_digit = &arg.value[0];
    while (1) {
        argval_int += (uint32_t)(*p_argval_digit - '0');

        p_argval_digit++;

        if (*p_argval_digit == '\0' || *p_argval_digit == '\n') {
            break;
        }

        argval_int *= 10;
    }

    return argval_int;
}

static uint32_t __setup_window_argument(command_arg_t arg, etc_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_ETC(WINDOW_WIDTH):
            p_arglist->window_resize.width = __parse_window_size_value(arg);
            break;

        case PARAM_ETC(WINDOW_HEIGHT):
            p_arglist->window_resize.height = __parse_window_size_value(arg);
            break;

        case PARAM_ETC(WINDOW_INSTANCE):
            p_arglist->window_display.p_instance = vulkan_obj_mgr_get_instance_object();
            // instance not created
            if (!p_arglist->window_display.p_instance) {
                printf("instance not created yet\n");
                return FAILURE;
            }

            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t etc_app_cmd_setup_argument_list(command_t *p_cmd, etc_cmd_args_list_t *p_arglist)
{
    uint32_t res;
    uint32_t (*__arg_setup_func)(command_arg_t, etc_cmd_args_list_t *);

    memset(p_arglist, 0, sizeof(etc_cmd_args_list_t));

    if (!strcmp(p_cmd->cmd_name, "window")) {
        __arg_setup_func = __setup_window_argument;
    }
    else {
        return FAILURE;
    }

    res = SUCCESS;
    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        res = __arg_setup_func(p_cmd->p_args[idx], p_arglist);
        if (res == FAILURE) {
            break;
        }
    }

    return res;
}

uint32_t etc_app_cmd_window_add_instance_obj(command_t *p_cmd)
{
    p_cmd->p_args[p_cmd->num_args].type = PARAM_ETC(WINDOW_INSTANCE);

    p_cmd->num_args++;

    return SUCCESS;
}

uint32_t etc_app_cmd_window_display(command_t *p_cmd)
{
    etc_cmd_args_list_t args_list;

    if (etc_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_obj_mgr_check_device_created() == FAILURE) {
        printf("device is not created\n");
        return FAILURE;
    }

    if (window_obj_mgr_setup_display(args_list.window_display.p_instance) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t etc_app_cmd_show_window_ctx_info(command_t *p_cmd)
{
    uint32_t phydev_count;
    VkPhysicalDevice *p_phydev;
    etc_cmd_args_list_t args_list;

    if (etc_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (window_obj_mgr_check_display_status() != WINDOW_OBJ_DISPLAY_STATE_CREATED) {
        printf("display not started yet\n");
        return FAILURE;
    }

    phydev_count = vulkan_obj_mgr_get_phydev_count();
    for (uint32_t idx = 0; idx < phydev_count; idx++) {
        p_phydev = vulkan_obj_mgr_get_phydev_object(idx);

        window_obj_mgr_show_display_ctx_info(p_phydev);
    }

    return SUCCESS;
}

static uint32_t etc_app_cmd_check_arg_exist(command_t *p_cmd, uint32_t arg_type)
{
    for (uint32_t idx = 0; idx <p_cmd->num_args; idx++) {
        if (p_cmd->p_args[idx].type == arg_type) {
            return SUCCESS;
        }
    }

    return FAILURE;
}

uint32_t etc_app_cmd_window_resize(command_t *p_cmd)
{
    etc_cmd_args_list_t args_list;

    if (etc_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return window_obj_mgr_resize(args_list.window_resize.width,
                                    args_list.window_resize.height);
}
