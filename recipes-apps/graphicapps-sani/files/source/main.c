#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

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

    res = window_obj_mgr_init(window_type, DEFAULT_WINDOW_SIZE_WIDTH, DEFAULT_WINDOW_SIZE_HEIGHT);
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
        printf("layer_count %u\n", layer_count);

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

void run(void)
{
    window_obj_mgr_start_display();

    command_t cmd;
input:
    printf("[CMD]: ");

    if (scanf("%s", cmd.input) != 1) {
        app_cmd_show_usage(&cmd);
        goto input;
    }

    if (app_cmd_process(&cmd) == FAILURE) {
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
