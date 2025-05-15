#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "../window_obj_mgr.hpp"

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
