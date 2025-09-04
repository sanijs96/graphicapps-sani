#ifndef __VULKAN_CMD_POOL_H__
#define __VULKAN_CMD_POOL_H__

#include <vulkan/vulkan_core.h>

enum pool_creation_state {
    VULKAN_CMD_POOL_STATE_DEFAULT,
    VULKAN_CMD_POOL_STATE_CREATED,
    NUM_VULKAN_CMD_POOL_STATES,
};

enum buffer_creation_state {
    VULKAN_CMD_POOL_CMDBUF_STATE_DEFAULT,
    VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED,
    VULKAN_CMD_POOL_CMDBUF_STATE_ACTIVATED,
    NUM_VULKAN_CMD_POOL_CMDBUF_STATES,
};

uint32_t cmd_pool_create(VkDevice *p_device, uint32_t queue_family_idx);
uint32_t cmd_pool_get_state(void);

uint32_t cmd_pool_allocate_buffer(VkDevice *p_device, uint32_t buf_type);
uint32_t cmd_pool_free_cmd_buffer(VkDevice *p_device, uint32_t buf_idx);
uint32_t cmd_pool_finish_buffer_recording(uint32_t buf_idx);
uint32_t cmd_pool_get_cmd_buffer_state(uint32_t buf_idx);
uint32_t cmd_pool_get_cmd_buffer_bitmap(uint32_t buf_idx);
VkCommandBuffer *cmd_pool_get_cmd_buffer_object(uint32_t buf_idx);

void cmd_pool_add_renderpass_command(vulkan_cmd_param_t *p_param);
void cmd_pool_add_bind_pipeline_command(vulkan_cmd_param_t *p_param);
void cmd_pool_add_bind_resource_command(vulkan_cmd_param_t *p_param);
void cmd_pool_add_draw_command(vulkan_cmd_param_t *p_param);
void cmd_pool_add_copy_resource_command(vulkan_cmd_param_t *p_param);

#endif
