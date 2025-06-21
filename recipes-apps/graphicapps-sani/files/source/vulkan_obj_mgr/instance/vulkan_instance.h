#ifndef __VULKAN_INSTANCE_H__
#define __VULKAN_INSTANCE_H__

#include <vulkan/vulkan_core.h>

enum instance_creation_state {
    VULKAN_INSTANCE_CREATION_STATE_DEFAULT = 0,
    VULKAN_INSTANCE_CREATION_STATE_CREATED,
    VULKAN_INSTANCE_CREATION_STATE_DELETED,
};

void instance_init(const char *app_name);
uint32_t instance_check_creation_state(void);

VkResult instance_create(void);
void instance_destroy(void);

uint32_t instance_get_physical_devices_count(void);
VkPhysicalDevice *instance_create_physical_device_list(uint32_t device_count);

#endif
