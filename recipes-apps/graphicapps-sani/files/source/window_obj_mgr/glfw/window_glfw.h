#ifndef __WINDOW_GLFW_H__
#define __WINDOW_GLFW_H__

#if defined(GLFW_INCLUDE_VULKAN)

#include <vulkan/vulkan_core.h>

#include <GLFW/glfw3.h>

void glfw_init(void);
uint32_t glfw_create_window(void);
uint32_t glfw_resize_window(uint32_t width, uint32_t height, VkInstance *p_instance);
uint32_t glfw_display_window(uint32_t width, uint32_t height, VkInstance *p_instance);
void glfw_destroy_window(VkInstance *p_instance);

#else
#error "Vulkan is not configured with GLFW"
#endif
#endif
