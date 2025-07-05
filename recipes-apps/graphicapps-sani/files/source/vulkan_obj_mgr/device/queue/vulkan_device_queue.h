#ifndef __VULKAN_DEVICE_QUEUE_H__
#define __VULKAN_DEVICE_QUEUE_H__

#include <vulkan/vulkan_core.h>

enum device_queue_state {
    VULKAN_DEVICE_QUEUE_STATE_USED,
    VULKAN_DEVICE_QUEUE_STATE_UNUSED,
};

void device_queue_init_ctx(void);
uint32_t device_queue_get_queue_count(uint32_t type);
uint32_t device_queue_get_total_queue_family_count(void);
uint32_t device_queue_get_queue_family_idx(uint32_t type);
uint32_t device_queue_enable_queue_family(uint32_t type, VkPhysicalDevice *p_phydev);
uint32_t device_queue_setup_queue_ctx(VkDeviceQueueCreateInfo *p_queues_info, uint32_t info_count);
VkQueue *device_queue_get_queue_objects(uint32_t type);
uint32_t device_queue_get_queue_state(uint32_t type, uint32_t queue_idx);


#endif
