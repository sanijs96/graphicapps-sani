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

enum device_function_state {
    VULKAN_DEVICE_FUNCTION_STATE_DISABLED = 0,
    VULKAN_DEVICE_FUNCTION_STATE_ENABLED,
};

typedef struct __device_queue_ctx {
    uint32_t *priorities;
    uint32_t num_queues;
} device_queue_ctx_t;

VkPhysicalDevice *device_create_physical_device_list(uint32_t count);
void device_register_physical_device_capabilities(void);

VkResult device_create(uint32_t phydev_idx, device_queue_ctx_t queue_info);

#endif
