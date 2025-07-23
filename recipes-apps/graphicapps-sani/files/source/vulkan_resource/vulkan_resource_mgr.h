#ifndef __VULKAN_RESOURCE_MGR_H__
#define __VULKAN_RESOURCE_MGR_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/resource_formats.h"

uint32_t vulkan_resource_mgr_create_vertex_buffer(VkDevice *p_device, resource_t *p_resources,
                                                                        resource_info_t *p_info);
uint32_t vulkan_resource_mgr_create_buffer(VkDevice *p_device, resource_t *p_resources,
                                                                        resource_info_t *p_info);
uint32_t vulkan_resource_mgr_create_image(VkDevice *p_device, resource_t *p_resources,
                                                                        resource_info_t *p_info);

resource_info_t *vulkan_resource_mgr_get_resource_info(char *resource_name);

void *vulkan_resource_mgr_get_resource_object(char *resource_name);

void vulkan_resource_mgr_add_memory_property(VkPhysicalDevice *p_phydev);

void vulkan_resource_mgr_show_device_memory_properties(void);

#endif
