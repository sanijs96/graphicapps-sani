#ifndef __EXTENSION_GLFW_H__
#define __EXTENSION_GLFW_H__

#include "glfw/glfw.hpp"
#include "extension/extension.hpp"

class glfwExtension: public vulkanExtension {
public:
    glfwExtension(void);
    uint32_t getRequiredExtensionCount(void);
    const char ** getRequiredExtensionNames(void);

private:
    const char **extensionNames;
    uint32_t extensionCount;
};

#endif
