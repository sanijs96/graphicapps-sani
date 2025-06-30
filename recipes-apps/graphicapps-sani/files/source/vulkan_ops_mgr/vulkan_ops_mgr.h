#ifndef __VULKAN_OPS_MGR_H__
#define __VULKAN_OPS_MGR_H__

#include <vulkan/vulkan_core.h>
#include "pipeline/vulkan_pipeline.h"

uint32_t vulkan_ops_mgr_create_pipeline(VkDevice *p_device);
uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device);
uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format);

VkRenderPass *vulkan_ops_mgr_get_renderpass_object(void);
#endif
