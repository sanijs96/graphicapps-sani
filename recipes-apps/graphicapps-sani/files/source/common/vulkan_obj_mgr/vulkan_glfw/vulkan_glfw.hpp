#if defined(GLFW_INCLUDE_VULKAN)

#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

class vulkanGlfwBinder {
public:
    vulkanGlfwBinder(void): glfwExtensionCount{0}, glfwExtensions{nullptr} { }
    void bindGlfwExtension(VkInstanceCreateInfo &);

private:
    uint32_t glfwExtensionCount;
    const char **glfwExtensions;
};

#endif