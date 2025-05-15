#include <vulkan/vulkan_core.h>

#if defined(GLFW_INCLUDE_VULKAN)
#include "vulkan_glfw/vulkan_glfw.hpp"
#endif

class vulkanObjectManager {
public:
    vulkanObjectManager(void): vkResult{VK_SUCCESS}, vkInstance{},
                                vkAppsInfo{}, vkInstanceInfo{} { }
    void createInstance(void);
    void destroyInstance(void);
    VkResult result(void);

private:
    VkResult vkResult;
    VkInstance vkInstance;
    VkApplicationInfo vkAppsInfo;
    VkInstanceCreateInfo vkInstanceInfo;
#if defined(GLFW_INCLUDE_VULKAN)
    vulkanGlfwBinder vkGlfwBinder;
#endif

    void fill_vkApplicationInfo();
};
