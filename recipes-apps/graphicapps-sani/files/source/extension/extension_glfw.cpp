#include "extension_glfw.hpp"

glfwExtension::glfwExtension(void)
: extensionCount{0}, extensionNames{nullptr}
{
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
}

uint32_t glfwExtension::getRequiredExtensionCount(void)
{
    return extensionCount;
}

const char **glfwExtension::getRequiredExtensionNames(void)
{
    return extensionNames;
}
