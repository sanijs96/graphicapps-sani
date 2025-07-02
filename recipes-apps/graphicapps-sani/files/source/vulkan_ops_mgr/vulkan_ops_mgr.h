#ifndef __VULKAN_OPS_MGR_H__
#define __VULKAN_OPS_MGR_H__

#include <vulkan/vulkan_core.h>
#include "pipeline/vulkan_pipeline.h"

uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device);
uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format);

uint32_t vulkan_ops_mgr_activate_pipeline(VkDevice *p_device);
uint32_t vulkan_ops_mgr_check_pipeline_state(void);
VkRenderPass *vulkan_ops_mgr_get_renderpass_object(void);

uint32_t vulkan_ops_mgr_allocate_cmd_buffer(VkDevice *p_device);

#endif
