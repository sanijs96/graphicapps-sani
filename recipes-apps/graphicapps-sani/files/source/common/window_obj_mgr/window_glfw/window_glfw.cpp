#include "window_glfw.hpp"

void glAppsWindow::createWindow(void)
{
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    windowObject = glfwCreateWindow(windowWidth, windowHeight,
                                    "Vulkan", nullptr, nullptr);
}

bool glAppsWindow::isCreated(void)
{
    return (windowObject == nullptr);
}


void glAppsWindow::changeSizeOfWindow(int width, int height)
{

}

void glAppsWindow::showWindow(void)
{
    while (!glfwWindowShouldClose(windowObject)) {
        glfwPollEvents();
    }
}

void glAppsWindow::destroyWindow(void)
{
    glfwDestroyWindow(windowObject);
    glfwTerminate();
}
