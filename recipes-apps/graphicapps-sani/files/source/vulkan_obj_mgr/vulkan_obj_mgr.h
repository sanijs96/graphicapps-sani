#ifndef __VULKAN_OBJ_MGR_H__
#define __VULKAN_OBJ_MGR_H__

#include <vulkan/vulkan_core.h>

void vulkan_obj_mgr_init(void);

uint32_t vulkan_obj_mgr_enable_layer(char *layer_name, uint32_t scope);
uint32_t vulkan_obj_mgr_disable_layer(char *layer_name, uint32_t scope);
uint32_t vulkan_obj_mgr_show_layers_list(void);

uint32_t vulkan_obj_mgr_enable_extension(char *extension_name, uint32_t scope);
uint32_t vulkan_obj_mgr_disable_extension(char *extension_name, uint32_t scope);
uint32_t vulkan_obj_mgr_show_extensions_list(void);

uint32_t vulkan_obj_mgr_create_instance(void);
VkInstance *vulkan_obj_mgr_get_instance_object(void);

uint32_t vulkan_obj_mgr_create_device(uint32_t phydev_idx);
uint32_t vulkan_obj_mgr_show_device_info(uint32_t phydev_idx);

void vulkan_obj_mgr_exit(void);

#endif
