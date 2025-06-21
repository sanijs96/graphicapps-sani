#include <string.h>

#include "common/common_def.h"

#include "etc_app_cmd.h"

#include "window_obj_mgr/window_obj_mgr.h"
#include "vulkan_obj_mgr/vulkan_obj_mgr.h"

typedef union etc_cmd_args_list {
    struct {
        uint32_t width;
        uint32_t height;
        VkInstance* p_instance;
    } window_display;

    struct {
        uint32_t width;
        uint32_t height;
        VkInstance* p_instance;
    } window_resize;

} etc_cmd_args_list_t;

static uint32_t __parse_window_size_value(command_arg_t arg)
{
    uint32_t argval_int;
    char *p_argval_digit;

    if ((arg.type != APPS_PARAM_TYPE_ETC_WINDOW_WIDTH) &&
        (arg.type != APPS_PARAM_TYPE_ETC_WINDOW_HEIGHT)) {
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
        case APPS_PARAM_TYPE_ETC_WINDOW_WIDTH:
            p_arglist->window_display.width = __parse_window_size_value(arg);
            break;

        case APPS_PARAM_TYPE_ETC_WINDOW_HEIGHT:
            p_arglist->window_display.height = __parse_window_size_value(arg);
            break;

        case APPS_PARAM_TYPE_ETC_WINDOW_INSTANCE:
            p_arglist->window_display.p_instance = vulkan_obj_mgr_get_instance_object();
            // instance not created
            if (!p_arglist->window_display.p_instance) {
                return FAILURE;
            }

            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t _etc_cmd_setup_argument_list(command_t *p_cmd, etc_cmd_args_list_t *p_arglist)
{
    uint32_t res;
    uint32_t (*__arg_setup_func)(command_arg_t, etc_cmd_args_list_t *);

    if (!strcmp(p_cmd->cmd_name, "window")) {
        __arg_setup_func = __setup_window_argument;
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

uint32_t _etc_cmd_window_add_instance_obj(command_t *p_cmd)
{
    while (p_cmd->p_args->value) {
        p_cmd->p_args++;
    }

    p_cmd->p_args->type = APPS_PARAM_TYPE_ETC_WINDOW_INSTANCE;
    p_cmd->p_args->value = (char *)malloc(sizeof(uint32_t)); // unused

    p_cmd->num_args++;

    return SUCCESS;
}

uint32_t _etc_cmd_window_display(command_t *p_cmd)
{
    etc_cmd_args_list_t args_list;

    memset(&args_list, 0, sizeof(etc_cmd_args_list_t));
    if (_etc_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return window_obj_mgr_start_display(args_list.window_display.width,
                                        args_list.window_display.height,
                                        args_list.window_display.p_instance);
}

uint32_t _etc_cmd_window_resize(command_t *p_cmd)
{
    etc_cmd_args_list_t args_list;

    memset(&args_list, 0, sizeof(etc_cmd_args_list_t));
    if (_etc_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.window_display.width == 0) {
        args_list.window_display.width = DEFAULT_WINDOW_SIZE_WIDTH;
    }

    if (args_list.window_display.height == 0) {
        args_list.window_display.height = DEFAULT_WINDOW_SIZE_HEIGHT;
    }

    return window_obj_mgr_resize(args_list.window_display.width,
                                    args_list.window_display.height,
                                    args_list.window_display.p_instance);
}

uint32_t _etc_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
