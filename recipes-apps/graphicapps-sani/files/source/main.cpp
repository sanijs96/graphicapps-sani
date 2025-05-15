#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "common/window_obj_mgr/window_glfw/glfw_window.hpp"
#include "common/vulkan_obj_mgr/vulkan_obj_mgr.hpp"

#include "triangle/triangle.hpp"

class graphicApps {
public:
    virtual void init(void) = 0;
    virtual void run(void) = 0;
    virtual void cleanup(void) = 0;
};

class glApps : public graphicApps {
public:
    void init(void);
    void run(void);
    void cleanup(void);

private:
    glAppsWindow window;
    vulkanObjectManager vulkan;
};

void glApps::init(void)
{
    window.createWindow();
    if (!window.isCreated()) {
        throw std::runtime_error("failed to create window object..");
    }

    vulkan.createInstance();
    if (vulkan.result() != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance..");
    }
}

void glApps::run(void)
{
    window.showWindow();
}

void glApps::cleanup(void)
{
    window.destroyWindow();
    vulkan.destroyInstance();
}

int main(void)
{
    glApps apps;

    apps.init();

    try {
        apps.run();
    }
    catch(const std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        apps.cleanup();

        return EXIT_FAILURE;
    }

    apps.cleanup();

    return EXIT_SUCCESS;
}
