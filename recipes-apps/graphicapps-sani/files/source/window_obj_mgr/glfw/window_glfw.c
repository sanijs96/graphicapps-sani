#include "common/common_def.h"
#include "window_glfw.h"

static struct glfw_window_context {
    uint32_t state;
    uint32_t width;
    uint32_t height;
    GLFWwindow *p_window_obj;
} glfw_window_ctx;

void glfw_init(void)
{
    glfwInit();
}

void glfw_create_window(void)
{
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void glfw_display_window(uint32_t width, uint32_t height)
{
    glfw_window_ctx.p_window_obj = glfwCreateWindow(width, height,
                                                    "Vulkan", NULL, NULL);

    //while (!glfwWindowShouldClose(glfw_window_ctx.p_window_obj)) {
    //    glfwPollEvents();
    //}
}

void glfw_destroy_window(void)
{
    glfwDestroyWindow(glfw_window_ctx.p_window_obj);
    glfwTerminate();
}

void glfw_resize_window(uint32_t width, uint32_t height)
{
    glfw_destroy_window();
    glfw_display_window(width, height);
}
