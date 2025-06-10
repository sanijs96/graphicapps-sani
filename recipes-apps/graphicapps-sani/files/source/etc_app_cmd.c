#include <string.h>

#include "common/common_def.h"

#include "etc_app_cmd.h"

typedef union etc_cmd_args_list {
    struct {
        uint32_t width;
        uint32_t height;
    } window_display;

    struct {
        uint32_t width;
        uint32_t height;
    } window_resize;

} etc_cmd_args_list_t;

static uint32_t __setup_window_argument(command_arg_t arg, etc_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case APPS_PARAM_TYPE_ETC_WINDOW_DISPLAY_WIDTH:
            p_arglist->window_display.width = (uint32_t)(*(char *)arg.value - '0');;
            break;

        case APPS_PARAM_TYPE_ETC_WINDOW_DISPLAY_HEIGHT:
            p_arglist->window_display.height = (uint32_t)(*(char *)arg.value - '0');;
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

uint32_t _etc_cmd_window_display(command_t *p_cmd)
{
    etc_cmd_args_list_t args_list;
    if (_etc_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    window_obj_mgr_start_display(args_list.window_display.width,
                                            args_list.window_display.height);

    return SUCCESS;
}

uint32_t _etc_cmd_window_resize(command_t *p_cmd)
{
    etc_cmd_args_list_t args_list;
    if (_etc_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    window_obj_mgr_start_display(args_list.window_display.width,
                                            args_list.window_display.height);

    return SUCCESS;
}

uint32_t _etc_cmd_console_exit(command_t *p_cmd)
{
    return SUCCESS;
}
