#include <malloc.h>
#include "common/common_def.h"
#include "window_glfw.h"

static struct glfw_window_context {
    uint32_t state;
    uint32_t width;
    uint32_t height;
    GLFWwindow *p_window;
    VkSurfaceKHR surface;
} glfw_window_ctx;

void glfw_init(void)
{
    glfwInit();
}

uint32_t glfw_create_window(void)
{
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

uint32_t glfw_display_window(uint32_t width, uint32_t height, VkInstance *p_instance)
{
    uint32_t res;

    glfw_window_ctx.p_window = glfwCreateWindow(width, height,
                                                    "Vulkan", NULL, NULL);

    res = glfwCreateWindowSurface(*p_instance, glfw_window_ctx.p_window,
                                            NULL, glfw_window_ctx.surface);

    if (res != VK_SUCCESS) {
        return res;
    }
    else {
        return SUCCESS;
    }

    //while (!glfwWindowShouldClose(glfw_window_ctx.p_window_obj)) {
    //    glfwPollEvents();
    //}
}

void glfw_destroy_window(VkInstance *p_instance)
{
    glfwDestroyWindow(glfw_window_ctx.p_window);

    if (!p_instance) {
        vkDestroySurfaceKHR(*p_instance, glfw_window_ctx.surface, NULL);
    }

    glfwTerminate();
}

uint32_t glfw_resize_window(uint32_t width, uint32_t height, VkInstance *p_instance)
{
    glfw_destroy_window(p_instance);

    return glfw_display_window(width, height, p_instance);
}
