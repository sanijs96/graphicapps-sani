#include <stdio.h>

#include "common/common_def.h"

#include "vulkan_ops_mgr.h"
#include "pipeline/vulkan_pipeline.h"
#include "cmd_buffer/vulkan_cmd_pool.h"

uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device)
{
    return pipeline_add_shader_file(stage, filename, p_device);
}

uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format)
{
    return pipeline_add_viewport_ctx(p_extent, p_format);
}

uint32_t vulkan_ops_mgr_activate_pipeline(VkDevice *p_device)
{
    return pipeline_create(p_device);
}

uint32_t vulkan_ops_mgr_check_pipeline_state(void)
{
    return pipeline_get_creation_state();
}

VkRenderPass *vulkan_ops_mgr_get_renderpass_object(void)
{
    return pipeline_get_renderpass_object();
}

uint32_t vulkan_ops_mgr_allocate_cmd_buffer(VkDevice *p_device)
{
    uint32_t res;
    uint32_t graphics_queue_idx;

    graphics_queue_idx = vulkan_obj_mgr_get_graphics_queue_idx(p_device);

    if (vulkan_cmd_pool_get_state() != VULKAN_CMD_POOL_STATE_CREATED) {
        res = vulkan_cmd_pool_create(p_device, graphics_queue_idx);
        if (res == FAILURE) {
            return res;
        }

    }

    res = vulkan_cmd_pool_allocate_buffer(p_device);

    return res;
}

