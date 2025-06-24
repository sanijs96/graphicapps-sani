#include <malloc.h>
#include "common/common_def.h"
#include "window_glfw.h"

static struct glfw_window_context {
    uint32_t state;
    uint32_t width;
    uint32_t height;
    GLFWwindow *p_window;
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

void glfw_change_window_size(uint32_t width, uint32_t height)
{
    glfw_window_ctx.width = width;
    glfw_window_ctx.height = height;
}


uint32_t glfw_display_window(VkSurfaceKHR *p_surface, VkInstance *p_instance)
{
    uint32_t res;

    glfw_window_ctx.p_window = glfwCreateWindow(glfw_window_ctx.width,
                                                glfw_window_ctx.height, "Vulkan", NULL, NULL);

    res = glfwCreateWindowSurface(*p_instance, glfw_window_ctx.p_window, NULL, p_surface);

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

void glfw_destroy_window(VkSurfaceKHR *p_surface, VkInstance *p_instance)
{
    glfwDestroyWindow(glfw_window_ctx.p_window);

    if (!p_instance) {
        vkDestroySurfaceKHR(*p_instance, *p_surface, NULL);
    }

    glfwTerminate();
}
