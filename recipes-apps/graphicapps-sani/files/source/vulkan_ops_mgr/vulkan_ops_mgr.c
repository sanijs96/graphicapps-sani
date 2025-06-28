#include <stdio.h>

#include "vulkan_ops_mgr.h"
#include "pipeline/vulkan_pipeline.h"

uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device)
{
    return pipeline_add_shader_file(stage, filename, p_device);
}

uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format)
{
    pipeline_add_viewport_ctx(p_extent, p_format);
}

uint32_t vulkan_ops_mgr_create_pipeline(VkDevice *p_device)
{
    pipeline_create(p_device);
}