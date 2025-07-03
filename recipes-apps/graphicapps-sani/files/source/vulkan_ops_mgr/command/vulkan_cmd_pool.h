#ifndef __VULKAN_CMD_POOL_H__
#define __VULKAN_CMD_POOL_H__

#include <vulkan/vulkan_core.h>

enum pool_creation_state {
    VULKAN_CMD_POOL_STATE_DEFAULT,
    VULKAN_CMD_POOL_STATE_CREATED,
    NUM_VULKAN_CMD_POOL_STATES,
};

#define MAX_NUM_CMD_BUFFERS       (4)

enum buffer_creation_state {
    VULKAN_CMD_POOL_CMDBUF_STATE_UNALLOCATED,
    VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED,
    VULKAN_CMD_POOL_CMDBUF_STATE_ACTIVATED,
    NUM_VULKAN_CMD_POOL_CMDBUF_STATES,
};

uint32_t vulkan_cmd_pool_create(VkDevice *p_device, uint32_t device_queue_idx);
uint32_t vulkan_cmd_pool_get_state(void);

uint32_t vulkan_cmd_pool_allocate_buffer(VkDevice *p_device);
uint32_t vulkan_cmd_pool_bind_cmd_buffer_to_pipeline(uint32_t buf_idx, VkPipeline *p_pipeline);
uint32_t vulkan_cmd_pool_get_cmd_buffer_state(uint32_t buf_idx);

void vulkan_cmd_pool_add_renderpass_command(vulkan_cmd_template_t *p_template,
                                                    vulkan_cmd_param_t *p_param);
void vulkan_cmd_pool_add_draw_command(vulkan_cmd_template_t *p_template,
                                                    vulkan_cmd_param_t *p_param);

#endif
