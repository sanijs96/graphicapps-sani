#if defined(GLFW_INCLUDE_VULKAN)

#include "vulkan_glfw.hpp"

void vulkanGlfwBinder::bindGlfwExtension(VkInstanceCreateInfo &createInfo)
{
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;
}

#endif