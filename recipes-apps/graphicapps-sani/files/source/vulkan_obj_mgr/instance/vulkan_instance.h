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
uint32_t check_instance_state(void);

// layers & extensions
uint32_t instance_enable_layer(char *layer_name);
uint32_t instance_disable_layer(char *layer_name);
uint32_t check_instance_layer_state(char *layer_name);

uint32_t instance_enable_extension(char *extension_name);
uint32_t instance_disable_extension(char *extension_name);
uint32_t check_instance_extension_state(char *extension_name);

VkResult instance_create(void);
VkResult instance_destroy(void);

uint32_t instance_get_physical_devices_count(void);
void instance_setup_physical_device_lists(VkPhysicalDevice *p_dev_list, uint32_t device_count);

#endif