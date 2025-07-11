#ifndef __VULKAN_DEVICE_QUEUE_H__
#define __VULKAN_DEVICE_QUEUE_H__

#include <vulkan/vulkan_core.h>

enum device_queue_state {
    VULKAN_DEVICE_QUEUE_STATE_USED,
    VULKAN_DEVICE_QUEUE_STATE_UNUSED,
    NUM_VULKAN_DEVICE_QUEUE_STATES,
};

enum device_queue_family_state {
    VULKAN_DEVICE_QUEUE_FAMILY_STATE_DISABLED,
    VULKAN_DEVICE_QUEUE_FAMILY_STATE_ENABLED,
    NUM_VULKAN_DEVICE_QUEUE_FAMILY_STATES,
};

enum device_queue_semaphores {
    VULKAN_DEVICE_QUEUE_SEMAPHORE_TYPE_WAIT,
    VULKAN_DEVICE_QUEUE_SEMAPHORE_TYPE_SIGNAL,
    NUM_VULKAN_DEVICE_QUEUE_SEMAPHORE_TYPES,
};

void device_queue_init_ctx(void);

uint32_t device_queue_setup_queue_create_info(VkDeviceQueueCreateInfo *p_queues_info);

uint32_t device_queue_get_total_queue_family_count(void);
uint32_t device_queue_get_queue_family_idx(uint32_t type);
uint32_t device_queue_get_queue_family_state(uint32_t type);
uint32_t device_queue_enable_queue_family(uint32_t type, VkPhysicalDevice *p_phydev);

uint32_t device_queue_get_queue_count(uint32_t type);
uint32_t device_queue_get_queue_state(uint32_t type, uint32_t idx);
VkQueue *device_queue_get_queue_object(uint32_t type, uint32_t idx);
uint32_t device_queue_enable_queues(uint32_t type, VkDevice *p_device);

uint32_t device_queue_lock(uint32_t type, uint32_t idx);
void device_queue_unlock(uint32_t type, uint32_t idx);

#endif
