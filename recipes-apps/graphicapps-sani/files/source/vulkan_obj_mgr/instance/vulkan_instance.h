#ifndef __VULKAN_INSTANCE_H__
#define __VULKAN_INSTANCE_H__

#include <vulkan/vulkan_core.h>

enum instance_function_state {
    VULKAN_INSTANCE_FUNCTION_STATE_DISABLED = 0,
    VULKAN_INSTANCE_FUNCTION_STATE_ENABLED,
};

enum instance_creation_state {
    VULKAN_INSTANCE_CREATION_STATE_DEFAULT = 0,
    VULKAN_INSTANCE_CREATION_STATE_CREATED,
    VULKAN_INSTANCE_CREATION_STATE_DELETED,
};

void instance_init_ctx(const char *app_name);
uint32_t instance_check_state(void);

// layers & extensions
uint32_t instance_get_num_layers(void);
uint32_t instance_enable_layer(char *layer_name);
uint32_t instance_disable_layer(char *layer_name);
uint32_t instance_check_layer_state(uint32_t layer_idx);
VkLayerProperties *instance_get_layer_info(uint32_t layer_idx);

uint32_t instance_get_num_extensions(void);
uint32_t instance_enable_extension(char *extension_name);
uint32_t instance_disable_extension(char *extension_name);
uint32_t instance_check_extension_state(uint32_t extension_idx);
VkExtensionProperties *instance_get_extension_info(uint32_t extension_idx);

VkResult instance_create(void);
void instance_destroy(void);

uint32_t instance_get_physical_devices_count(void);
VkPhysicalDevice *instance_create_physical_device_list(uint32_t device_count);

#endif
