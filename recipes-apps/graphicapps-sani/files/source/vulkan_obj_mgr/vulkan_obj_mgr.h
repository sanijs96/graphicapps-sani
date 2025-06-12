#ifndef __VULKAN_OBJ_MGR_H__
#define __VULKAN_OBJ_MGR_H__

#include <vulkan/vulkan_core.h>

enum vulkan_function_scope {
    VULKAN_FUNCTION_SCOPE_INSTANCE = 0,
    VULKAN_FUNCTION_SCOPE_DEVICE = 1,
};

void vulkan_obj_mgr_init_instance_ctx(void);
void vulkan_obj_mgr_init_device_ctx(void);

uint32_t vulkan_obj_mgr_enable_layer(char *layer_name, uint32_t scope);
uint32_t vulkan_obj_mgr_disable_layer(char *layer_name, uint32_t scope);
uint32_t vulkan_obj_mgr_show_layers_list(void);

uint32_t vulkan_obj_mgr_enable_extension(char *extension_name, uint32_t scope);
uint32_t vulkan_obj_mgr_disable_extension(char *extension_name, uint32_t scope);
uint32_t vulkan_obj_mgr_show_extensions_list(void);

uint32_t vulkan_obj_mgr_create_instance(void);

uint32_t vulkan_obj_mgr_create_device(uint32_t phydev_idx);
uint32_t vulkan_obj_mgr_show_devices_list(void);

void vulkan_obj_mgr_exit(void);

#endif
