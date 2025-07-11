#ifndef __VULKAN_OPS_MGR_H__
#define __VULKAN_OPS_MGR_H__

#include <vulkan/vulkan_core.h>

uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device);
uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format);

uint32_t vulkan_ops_mgr_create_pipeline(VkDevice *p_device);
uint32_t vulkan_ops_mgr_check_pipeline_created(void);
uint32_t vulkan_ops_mgr_run_pipeline(VkQueue *p_queue);
VkPipeline *vulkan_ops_mgr_get_pipeline_object(void);
VkRenderPass *vulkan_ops_mgr_get_renderpass_object(void);

uint32_t vulkan_ops_mgr_check_cmd_buffer_allocated(uint32_t buf_idx);
uint32_t vulkan_ops_mgr_allocate_cmd_buffer(VkDevice *p_device, uint32_t graphics_queue_idx);
uint32_t vulkan_ops_mgr_activate_cmd_buffer(uint32_t cmdbuf_idx, VkPipeline *p_pipeline);
VkCommandBuffer *vulkan_ops_mgr_get_activated_cmd_buffer_object(void);

uint32_t vulkan_ops_mgr_add_vulkan_command(uint32_t cmd_type, vulkan_cmd_param_t *p_param);

#endif
