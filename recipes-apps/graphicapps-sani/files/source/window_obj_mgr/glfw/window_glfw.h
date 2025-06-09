#ifndef __WINDOW_GLFW_H__
#define __WINDOW_GLFW_H__

#if defined(GLFW_INCLUDE_VULKAN)

#include <vulkan/vulkan_core.h>

#include <GLFW/glfw3.h>

void glfw_init(void);
void glfw_create_window(uint32_t width, uint32_t height);
void glfw_resize_window(uint32_t width, uint32_t height);
void glfw_display_window(void);
void glfw_destroy_window(void);

#else
#error "Vulkan is not configured with GLFW"
#endif

#endif
