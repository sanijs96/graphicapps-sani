#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>

#define DEBUG_EN
#define GLFW_INCLUDE_VULKAN

#include "apps/apps.hpp"

#include "layer/layer_check.hpp"
#include "extension/extension.hpp"

#if defined (DEBUG_EN)
#include "debug/debug.hpp"
#endif

#if defined(GLFW_INCLUDE_VULKAN)
#include "window/window_glfw.hpp"
#include "extension/extension_glfw.hpp"
#endif

#include "vulkan_obj_mgr/vulkan_obj_mgr.hpp"

class glApps : public Apps {
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
    if (vulkan.result != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance..");
    }

#if defined(GLFW_INCLUDE_VULKAN)
    uint32_t extensionCount;
    const char **extensionNames;
    glfwExtension glfwExtension;

    extensionNames = glfwExtension.getRequiredExtensionNames();

    extensionCount = glfwExtension.getRequiredExtensionCount();

    for (uint32_t idx = 0; idx < extensionCount; idx++) {
         std::cout << "ext:" << extensionNames[idx] << std::endl;

         vulkan.addInstanceExtensionInfo(extensionNames[idx]);
    }
#endif

#if defined(DEBUG_EN)
    if (enableValidationLayers == true) {
        uint32_t layerCount;
        vulkanLayer vkLayer;

        layerCount = vulkan.getEnabledInstanceLayerCount();

        for (auto &layerName : validationLayerNamesList) {
            if (vkLayer.checkLayerSupport(layerName) == true) {
                vulkan.addInstanceLayerInfo(layerName);
            }
        }

        if (vulkan.getEnabledInstanceLayerCount() == layerCount) {
            throw std::runtime_error("no validation layer enabled..");
        }

        vulkan.addInstanceExtensionInfo(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#endif

    vulkan.selectGPUDevice();
    if (vulkan.result != VK_SUCCESS) {
        throw std::runtime_error("device creation failure..");
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
