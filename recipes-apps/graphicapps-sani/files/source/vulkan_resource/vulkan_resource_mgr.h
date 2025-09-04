#ifndef __VULKAN_RESOURCE_MGR_H__
#define __VULKAN_RESOURCE_MGR_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/resource_formats.h"

uint32_t vulkan_resource_mgr_add_resource_info(resource_info_t *p_info);

uint32_t vulkan_resource_mgr_get_resource_data_unit_size(uint32_t type);

uint32_t vulkan_resource_mgr_create_vertex_buffer(VkDevice *p_device, char *resource_name,
                                                    resource_member_list_t *p_members_list);
uint32_t vulkan_resource_mgr_create_index_buffer(VkDevice *p_device, char *resource_name,
                                                    resource_member_list_t *p_members_list);
uint32_t vulkan_resource_mgr_create_image(VkDevice *p_device, char *resource_name,
                                                    resource_member_list_t *p_members_list);

resource_info_t *vulkan_resource_mgr_get_resource_info(char *resource_name);

uint32_t vulkan_resource_mgr_get_resource_description(resource_info_t *p_info,
                                                        resource_description_t *p_description);

uint32_t vulkan_resource_mgr_get_resource_state(char *resource_name);

uint32_t vulkan_resource_mgr_add_binding_cmdbuf_idx(char *resource_name, uint32_t buf_idx);

uint32_t vulkan_resource_mgr_get_binding_vertex_count(uint32_t cmdbuf_idx);

uint32_t vulkan_resource_mgr_get_binding_index_count(uint32_t cmdbuf_idx);

void *vulkan_resource_mgr_get_resource_object(char *resource_name);

void *vulkan_resource_mgr_create_device_resource_copy(VkDevice *p_device, char *resource_name);

void vulkan_resource_mgr_add_memory_property(VkPhysicalDevice *p_phydev);

void vulkan_resource_mgr_show_device_memory_properties(void);

#endif
