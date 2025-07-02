#ifndef __VULKAN_CMD_POOL_H__
#define __VULKAN_CMD_POOL_H__

#include <vulkan/vulkan_core.h>

#define MAX_NUM_CMD_BUFFERS       (4)

enum pool_creation_state {
    VULKAN_CMD_POOL_STATE_DEFAULT,
    VULKAN_CMD_POOL_STATE_CREATED,
    NUM_VULKAN_CMD_POOL_STATES,
};

uint32_t vulkan_cmd_pool_create(VkDevice *p_device, uint32_t device_queue_idx);
uint32_t vulkan_cmd_pool_get_state(void);

uint32_t vulkan_cmd_pool_allocate_buffer(VkDevice *p_device);

#endif
