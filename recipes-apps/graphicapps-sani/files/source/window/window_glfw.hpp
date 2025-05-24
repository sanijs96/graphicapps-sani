#ifndef __WINDOW_GLFW_H__
#define __WINDOW_GLFW_H__

#include <vulkan/vulkan_core.h>

#include <glfw/glfw.hpp>
#include <window/window_obj.hpp>

class glAppsWindow:public graphicAppsWindow {
public:
    glAppsWindow(void): windowWidth{WINDOW_SIZE_WIDTH}, windowHeight{WINDOW_SIZE_HEIGHT} {};
    glAppsWindow(int width, int height): windowWidth{height}, windowHeight{height} {}
    void createWindow(void);
    bool isCreated(void);
    void changeSizeOfWindow(int width, int height);
    void showWindow(void);
    void destroyWindow(void);

private:
    int windowWidth;
    int windowHeight;
    GLFWwindow *windowObject;
};

#endif
