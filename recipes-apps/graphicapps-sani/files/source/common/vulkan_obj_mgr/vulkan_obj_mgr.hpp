#ifndef __VULKAN_OBJ_MGR_H__
#define __VULKAN_OBJ_MGR_H__

#include <vulkan/vulkan_core.h>

#if defined(GLFW_INCLUDE_VULKAN)
#include "vulkan_glfw/vulkan_glfw.hpp"
#endif

#include "layers/layer_obj_mgr.hpp"

class vulkanObjectManager {
public:
    vulkanObjectManager(void): vkResult{VK_SUCCESS}, vkInstance{},
                                vkAppsInfo{}, vkInstanceInfo{} { }
    void createInstance(void);
    void destroyInstance(void);
    void addInstanceLayerInfo(void);
    bool checkLayerSupport(const char *layerName);
    VkResult result(void);

private:
    VkResult vkResult;
    VkInstance vkInstance;
    VkApplicationInfo vkAppsInfo;
    VkInstanceCreateInfo vkInstanceInfo;
    vulkanLayer vkLayers;
#if defined(GLFW_INCLUDE_VULKAN)
    vulkanGlfwBinder vkGlfwBinder;
#endif

    void fill_vkApplicationInfo();
};

#endif