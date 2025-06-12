#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "common/common_def.h"

#if defined (DEBUG_EN)
#include "debug/debug.h"
#endif

#if defined(GLFW_INCLUDE_VULKAN)
#include <GLFW/glfw3.h>
#endif

#include "app_cmd_handler.h"

#include "window_obj_mgr/window_obj_mgr.h"
#include "vulkan_obj_mgr/vulkan_obj_mgr.h"

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

    vulkan_obj_mgr_init_instance_ctx();

#if defined(DEBUG_EN)
    if (enable_validation == TRUE) {
        char *layer_name;

        uint32_t layer_count;
        layer_count = sizeof(validation_layer_names_list) / sizeof(const char *);

        for (uint32_t idx = 0; idx < layer_count; idx++) {
            layer_name = (char *)validation_layer_names_list[idx];

            res = vulkan_obj_mgr_enable_layer(layer_name, VULKAN_FUNCTION_SCOPE_INSTANCE);
            if (res == FAILURE) {
                printf("layer enable failure\n");

                goto exit;
            }
        }

        res = vulkan_obj_mgr_enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                                                    VULKAN_FUNCTION_SCOPE_INSTANCE);
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
        res = vulkan_obj_mgr_enable_extension(glfw_ext_names[idx],
                                    VULKAN_FUNCTION_SCOPE_INSTANCE);
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

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++ ) {
        free(p_cmd->p_args[idx].value);
    }

    free(p_cmd->p_args);
}

#define __shift_cursor(str, len)        (*str = ((char *)*str) + len)

static uint32_t __get_next_arg_length(char **cursor)
{
    uint32_t input_length;
    char *delim;

    input_length = 0;
    while (delim = strstr(*cursor, " ")) {
        input_length = (uint32_t)(delim - *cursor);

        // duplicated space
        if (input_length == 0) {
            __shift_cursor(cursor, 1);
            continue;
        }

        return input_length;
    }

    delim = strstr(*cursor, "\n");
    if (delim) {
        input_length = (uint32_t)(delim - *cursor);
    }
    else {
        delim = strstr(*cursor, "\0");
        if (delim) {
            input_length = strlen(*cursor);
        }
    }

    return input_length;
}

static uint32_t __setup_args_list(command_t *p_cmd, char* input)
{
    uint32_t res;
    uint32_t max_num_args;
    uint32_t input_length;
    char **input_cursor;

    input_cursor = &input;

    input_length = __get_next_arg_length(input_cursor);
    if (input_length == 0) {
        res = FAILURE;
    }

    p_cmd->cmd_name = strndup(*input_cursor, input_length);
    __shift_cursor(input_cursor, input_length);

    input_length = __get_next_arg_length(input_cursor);
    if (input_length == 0) {
        res = FAILURE;
    }

    p_cmd->subcmd_name = strndup(*input_cursor, input_length);
    __shift_cursor(input_cursor, input_length);

    max_num_args = app_cmd_check_max_num_cmd_args(p_cmd);
    if (max_num_args == 0) {
        res = SUCCESS;
    }

    p_cmd->p_args = (command_arg_t *)malloc(sizeof(command_arg_t) * max_num_args);

    p_cmd->num_args = 0;
    while (max_num_args > p_cmd->num_args) {
        *input_cursor = strstr(*input_cursor, "-");
        if (*input_cursor == NULL) {
            break;
        }
        __shift_cursor(input_cursor, 1); // skip dellimiter '-'
        p_cmd->p_args[p_cmd->num_args].type = **input_cursor;

        __shift_cursor(input_cursor, 1);

        input_length = __get_next_arg_length(input_cursor);
        if (input_length == 0) {
            break;
        }
        p_cmd->p_args[p_cmd->num_args].value = strndup(*input_cursor, input_length);

        __shift_cursor(input_cursor, input_length);

        p_cmd->num_args++;
    }

    printf("args: <");
    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++ ) {
        printf(" %s", p_cmd->p_args[idx].value);
    } printf(" >\n");

    res = SUCCESS;

exit:
    return res;
}

void run(void)
{
    window_obj_mgr_start_display(DEFAULT_WINDOW_SIZE_WIDTH, DEFAULT_WINDOW_SIZE_HEIGHT);

    command_t cmd = {0, };
    char input[MAX_LENGTH_APP_CMD];
input:
    __cleanup_args_list(&cmd);

    printf("[CMD]: ");

    fgets(input, MAX_LENGTH_APP_CMD, stdin);

    if (__setup_args_list(&cmd, input) == FAILURE) {
        printf("arguments not valid\n");
        app_cmd_show_usage(&cmd);
        goto input;
    }

    if (app_cmd_process(&cmd) == FAILURE) {
        printf("cmd failed\n");
        app_cmd_show_usage(&cmd);
    }

    if (!app_cmd_check_exited(&cmd)) {
        goto input;
    }

    return;
}

void cleanup(void)
{
    window_obj_mgr_exit();
    vulkan_obj_mgr_exit();
}


int main(void)
{
    init_components();

    run();

    cleanup();

    return EXIT_SUCCESS;
}
