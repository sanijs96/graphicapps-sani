#ifndef __VULKAN_DEVICE_H__
#define __VULKAN_DEVICE_H__

#include <vulkan/vulkan_core.h>

enum device_creation_state {
    VULKAN_DEVICE_CREATION_STATE_DEFAULT = 0,
    VULKAN_DEVICE_CREATION_STATE_CREATED,
    VULKAN_DEVICE_CREATION_STATE_DELETED,
};

enum device_queue_priority {
    VULKAN_DEVICE_QUEUE_PRIO_LOW,
    VULKAN_DEVICE_QUEUE_PRIO_MIDDLE,
    VULKAN_DEVICE_QUEUE_PRIO_HIGH,
    VULKAN_DEVICE_QUEUE_PRIO_MAX,

    VULKAN_DEVICE_QUEUE_PRIO_DEFAULT = VULKAN_DEVICE_QUEUE_PRIO_MIDDLE,
};

enum device_queue_type {
    VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS,
    VULKAN_DEVICE_QUEUE_TYPE_COMPUTE,
    VULKAN_DEVICE_QUEUE_TYPE_TRANSFER,
    NUM_VULKAN_DEVICE_QUEUE_TYPES,
};

enum device_queue_utility_status {
    VULKAN_DEVICE_QUEUE_USED,
    VULKAN_DEVICE_QUEUE_UNUSED,
};

typedef struct __device_queue_ctx {
    uint32_t *priorities;
    uint32_t num_queues;
} device_queue_ctx_t;

void device_create_physical_device_ctx(uint32_t count);
void device_register_physical_device(VkPhysicalDevice *p_device, uint32_t phydev_idx);
void device_register_device_capability(uint32_t phydev_idx);

void device_add_enabled_exts_info(uint32_t ext_count, char **extensions_name_list);
VkResult device_create(uint32_t phydev_idx);

uint32_t device_get_phydevs_count(void);

uint32_t device_get_current_status(void);
uint32_t device_get_current_phydev_idx(void);
VkDevice * device_get_current_device_object(void);
VkPhysicalDevice * device_get_phydev_object(uint32_t phydev_idx);

VkPhysicalDeviceProperties *device_get_device_property(uint32_t phydev_idx);

uint32_t device_get_device_queue_property_count(uint32_t phydev_idx);
uint32_t device_get_queue_idx(uint32_t queue_type);

VkQueueFamilyProperties *device_get_device_queue_property(uint32_t phydev_idx);

#endif
