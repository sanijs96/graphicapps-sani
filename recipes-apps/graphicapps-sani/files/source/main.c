#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan/debug.h"
#include "vulkan/cmd_types.h"

#if defined(GLFW_INCLUDE_VULKAN)
#include <GLFW/glfw3.h>
#endif

#include "app_cmd_handler.h"

#include "app_utilities/datascript_parser.h"

#include "vulkan_object/vulkan_obj_mgr.h"
#include "vulkan_operation/vulkan_ops_mgr.h"
#include "vulkan_resource/vulkan_resource_mgr.h"

#include "window_object/window_obj_mgr.h"

uint32_t __select_window_obj_type(void)
{
#if defined(GLFW_INCLUDE_VULKAN)
    return WINDOW_OBJ_TYPE_GLFW;
#endif

    return WINDOW_OBJ_TYPE_NONE;
}

uint32_t init_components(void)
{
    uint32_t res;
    uint32_t window_type;

    window_type = __select_window_obj_type();

    res = window_obj_mgr_init(window_type);
    if (res == FAILURE) {
        printf("window creation failure\n");

        goto exit;
    }

    vulkan_resource_mgr_init();

    vulkan_obj_mgr_init();

#if defined(DEBUG_EN)
    if (enable_validation == TRUE) {
        char *layer_name;

        uint32_t layer_count;
        layer_count = sizeof(validation_layer_names_list) / sizeof(const char *);

        for (uint32_t idx = 0; idx < layer_count; idx++) {
            layer_name = (char *)validation_layer_names_list[idx];

            res = vulkan_obj_mgr_enable_layer(layer_name);
            if (res == FAILURE) {
                printf("layer enable failure\n");

                goto exit;
            }
        }

        res = vulkan_obj_mgr_enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        if (res == FAILURE) {
            printf("extension enable failure\n");

            goto exit;
        }
    }
#endif

#if defined(GLFW_INCLUDE_VULKAN)
    uint32_t glfw_ext_count;
    char **glfw_ext_names;

    glfw_ext_count = 0;
    glfw_ext_names = (char **)glfwGetRequiredInstanceExtensions(&glfw_ext_count);

    for (uint32_t idx = 0; idx < glfw_ext_count; idx++) {
        res = vulkan_obj_mgr_enable_extension(glfw_ext_names[idx]);
        if (res == FAILURE) {
            printf("glfw extension enable failure\n");

            goto exit;
        }
    }
#endif

exit:
    return res;
}

static void __cleanup_args_list(command_t *p_cmd)
{
    if (p_cmd->num_args == 0) {
        return;
    }

    free(p_cmd->p_args);

    memset(p_cmd, 0, sizeof(command_t));
}

#define __shift_cursor(str, len)        (*str = ((char *)*str) + len)

static uint32_t __get_next_arg_length(char **cursor)
{
    uint32_t input_length;
    char *delim;

    input_length = 0;
    while (delim = strstr(*cursor, " " "\t")) {
        input_length = (uint32_t)(delim - *cursor);

        // duplicated space
        if (input_length == 0) {
            __shift_cursor(cursor, 1);
            continue;
        }

        return input_length;
    }

    delim = strchr(*cursor, '\n');
    if (delim) {
        input_length = (uint32_t)(delim - *cursor);
    }
    else {
        delim = strchr(*cursor, '\0');
        if (delim) {
            input_length = strlen(*cursor);
        }
    }

    return input_length;
}

static uint32_t __setup_argval(char *p_argval_buf, char **input_cursor)
{
    uint32_t input_length;

    input_length = __get_next_arg_length(input_cursor);
    if (input_length == 0) {
        p_argval_buf = NULL;
        return FAILURE;
    }

    memset(p_argval_buf, *input_cursor, strlen(p_argval_buf));

    strncpy(p_argval_buf, *input_cursor, input_length);

    __shift_cursor(input_cursor, input_length);

    return SUCCESS;
}

static uint32_t __setup_args_list(command_t *p_cmd, char* input)
{
    uint32_t res;
    uint32_t arg_idx;
    uint32_t max_num_args;
    uint32_t input_length;
    char **input_cursor;

    input_cursor = &input;

    if (strchr(*input_cursor, '=') != NULL) {
        if (__setup_argval(p_cmd->saved_argname, input_cursor) == FAILURE) {
            return FAILURE;
        }

        *input_cursor = strchr(*input_cursor, '=');

        __shift_cursor(input_cursor, 1); // skip assignment character '='
    }
    else {
        p_cmd->saved_argname[0] = '\0';
    }

    if (__setup_argval(p_cmd->cmd_name, input_cursor) == FAILURE) {
        return FAILURE;
    }

    if (__setup_argval(p_cmd->subcmd_name, input_cursor) == FAILURE) {
        return FAILURE;
    }

    max_num_args = app_cmd_handler_check_max_num_cmd_args(p_cmd);
    if (max_num_args == 0) {
        res = SUCCESS;
    }

    p_cmd->p_args = (command_arg_t *)malloc(sizeof(command_arg_t) * max_num_args);

    arg_idx = 0;
    while (max_num_args > arg_idx) {
        memset(p_cmd->p_args[arg_idx].value, 0, MAX_LENGTH_APP_CMD);

        *input_cursor = strchr(*input_cursor, '-');
        if (*input_cursor == NULL) {
            break;
        }
        __shift_cursor(input_cursor, 1); // skip delimiter '-'

        p_cmd->p_args[arg_idx].type = **input_cursor;
        __shift_cursor(input_cursor, 1);

        if (__setup_argval(p_cmd->p_args[arg_idx].value, input_cursor) == FAILURE) {
            return FAILURE;
        }

        arg_idx++;
    }

    p_cmd->num_args = arg_idx;

    res = SUCCESS;

exit:
    return res;
}

void run(void)
{
    command_t cmd = {0, };
    char p_input_str[MAX_LENGTH_APP_CMD];

input:
    __cleanup_args_list(&cmd);

    if (datascript_check_scriptfile_registered() == FALSE) {
        printf("[CMD]: ");
        fgets(p_input_str, MAX_LENGTH_APP_CMD, stdin);
    }
    else if (datascript_get_command(p_input_str) == FAILURE) {
        goto input;
    }

    if (__setup_args_list(&cmd, p_input_str) == FAILURE) {
        printf("arguments not valid\n");
        goto input;
    }

    if (app_cmd_handler_process(&cmd) == FAILURE) {
        printf("cmd failed\n");
        app_cmd_handler_show_usage(&cmd);
    }

    if (cmd.saved_argname[0] != '\0') {
        datascript_save_command_result(cmd.saved_argname, cmd.retval);
    }

    if (!app_cmd_handler_check_exited(&cmd)) {
        goto input;
    }

    return;
}

void cleanup(void)
{
    window_obj_mgr_exit(NULL);
    vulkan_obj_mgr_exit();
}


int main(void)
{
    init_components();

    run();

    cleanup();

    return EXIT_SUCCESS;
}
