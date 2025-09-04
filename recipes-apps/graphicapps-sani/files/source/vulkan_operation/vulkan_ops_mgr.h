#ifndef __VULKAN_OPS_MGR_H__
#define __VULKAN_OPS_MGR_H__

#include <vulkan/vulkan_core.h>

uint32_t vulkan_ops_mgr_get_vulkan_cmd_buffer_type(const char *p_type_name);
uint32_t vulkan_ops_mgr_get_vulkan_cmd_type_from_name(const char *p_cmd_name);
uint32_t vulkan_ops_mgr_get_pipeline_stage_idx(const char *p_stage_name);

// pipeline setup
uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device);
uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format);
uint32_t vulkan_ops_mgr_add_vertex_input_ctx(resource_description_t *p_description);

uint32_t vulkan_ops_mgr_get_pipeline_idx_setup_in_progress(void);
uint32_t vulkan_ops_mgr_init_pipeline_setup(void);
uint32_t vulkan_ops_mgr_create_pipeline(VkDevice *p_device, uint32_t pipeline_entry_idx);
VkPipeline *vulkan_ops_mgr_get_pipeline_object(uint32_t idx);
VkRenderPass *vulkan_ops_mgr_get_renderpass_object(void);

// command buffer setup
uint32_t vulkan_ops_mgr_check_cmd_buffer_allocated(uint32_t buf_idx);
uint32_t vulkan_ops_mgr_check_cmd_buffer_activated(uint32_t buf_idx);
uint32_t vulkan_ops_mgr_get_cmd_buffer_bitmap(uint32_t buf_idx);
uint32_t vulkan_ops_mgr_allocate_cmd_buffer(VkDevice *p_device, uint32_t family_idx,
                                                                    uint32_t buf_type);
uint32_t vulkan_ops_mgr_activate_cmd_buffer(uint32_t cmdbuf_idx);
VkCommandBuffer *vulkan_ops_mgr_get_activated_cmd_buffer_object(void);
uint32_t vulkan_ops_mgr_free_cmd_buffer(VkDevice *p_device, uint32_t buf_idx);

// command handler setup
uint32_t vulkan_ops_mgr_get_draw_cmd_subcmd_type(char *subcmd_name);

uint32_t vulkan_ops_mgr_add_vulkan_command(uint32_t cmd_type, vulkan_cmd_param_t *p_param);

#endif
