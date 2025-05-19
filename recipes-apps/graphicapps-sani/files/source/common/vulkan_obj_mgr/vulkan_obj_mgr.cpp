#include "vulkan_obj_mgr.hpp"
#include "layers/layer_obj_mgr.hpp"

void vulkanObjectManager::fill_vkApplicationInfo(void)
{
    vkAppsInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    vkAppsInfo.pApplicationName = "Sani Vulkan Application";
    vkAppsInfo.pEngineName = "No Engine";

    vkAppsInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vkAppsInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppsInfo.apiVersion = VK_API_VERSION_1_0;

    vkAppsInfo.pNext = nullptr; // used on pointing to next vulkan extension object
}

void vulkanObjectManager::createInstance(void)
{
    fill_vkApplicationInfo();

    vkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceInfo.pApplicationInfo = &vkAppsInfo;

#if defined(GLFW_USING_VULKAN)
    vlGlfwBinder.bindGlfwExtension(vkInstanceInfo);
#endif

    vkResult = vkCreateInstance(&vkInstanceInfo, nullptr, &vkInstance);
}

void vulkanObjectManager::destroyInstance(void)
{
    vkDestroyInstance(vkInstance, nullptr);
}

void vulkanObjectManager::addInstanceLayerInfo(void)
{
    vkInstanceInfo.enabledLayerCount = vkLayers.getSupportedLayerCount();

    vkLayers.addLayerNamesTo(vkInstanceInfo.ppEnabledLayerNames);
}

bool vulkanObjectManager::checkLayerSupport(const char *layerName)
{
    return vkLayers.checkLayerSupport(layerName);
}

VkResult vulkanObjectManager::result(void)
{
    return vkResult;
}

