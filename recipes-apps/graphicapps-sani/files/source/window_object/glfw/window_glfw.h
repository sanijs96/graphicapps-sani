#ifndef __WINDOW_GLFW_H__
#define __WINDOW_GLFW_H__

#if defined(GLFW_INCLUDE_VULKAN)

#include <vulkan/vulkan_core.h>

#include <GLFW/glfw3.h>

void glfw_init(void);
uint32_t glfw_create_window(void);
void glfw_change_window_size(uint32_t width, uint32_t height);
uint32_t glfw_display_window(VkSurfaceKHR *p_surface,VkInstance *p_instance);
void glfw_destroy_window(VkSurfaceKHR *p_surface, VkInstance *p_instance);

#else
#error "Vulkan is not configured with GLFW"
#endif
#endif
