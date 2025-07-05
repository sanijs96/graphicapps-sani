#ifndef __VULKAN_OBJ_MGR_H__
#define __VULKAN_OBJ_MGR_H__

#include <vulkan/vulkan_core.h>

void vulkan_obj_mgr_init(void);

uint32_t vulkan_obj_mgr_enable_layer(char *layer_name);
uint32_t vulkan_obj_mgr_disable_layer(char *layer_name);
void vulkan_obj_mgr_show_layers_list(void);

uint32_t vulkan_obj_mgr_enable_extension(char *extension_name);
uint32_t vulkan_obj_mgr_disable_extension(char *extension_name);
void vulkan_obj_mgr_show_extensions_list(void);

uint32_t vulkan_obj_mgr_enable_phydev_layer(char *extension_name, uint32_t phydev_idx);
uint32_t vulkan_obj_mgr_disable_phydev_layer(char *extension_name, uint32_t phydev_idx);
uint32_t vulkan_obj_mgr_enable_phydev_extension(char *extension_name, uint32_t phydev_idx);
uint32_t vulkan_obj_mgr_disable_phydev_extension(char *extension_name, uint32_t phydev_idx);

uint32_t vulkan_obj_mgr_create_instance(void);
VkInstance *vulkan_obj_mgr_get_instance_object(void);

uint32_t vulkan_obj_mgr_get_phydev_count(void);
uint32_t vulkan_obj_mgr_get_current_phydev_idx(void);
VkDevice *vulkan_obj_mgr_get_current_device_object(void);
VkPhysicalDevice *vulkan_obj_mgr_get_phydev_object(uint32_t phydev_idx);

uint32_t vulkan_obj_mgr_create_device(uint32_t phydev_idx);
uint32_t vulkan_obj_mgr_check_device_created(void);

uint32_t vulkan_obj_mgr_get_graphics_queue_idx(VkDevice *p_device);
uint32_t vulkan_obj_mgr_get_compute_queue_idx(VkDevice *p_device);
uint32_t vulkan_obj_mgr_get_transfer_queue_idx(VkDevice *p_device);

void vulkan_obj_mgr_show_device_info(uint32_t phydev_idx);
void vulkan_obj_mgr_show_device_extensions_list(uint32_t phydev_idx);

void vulkan_obj_mgr_exit(void);

#endif
