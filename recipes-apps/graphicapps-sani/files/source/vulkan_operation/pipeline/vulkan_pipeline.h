#ifndef __VULKAN_PIPELINE_H__
#define __VULKAN_PIPELINE_H__

#include <vulkan/vulkan_core.h>

#define MAX_NUM_VULKAN_PIPELINES    (8)

enum pipeline_type {
    VULKAN_PIPELINE_TYPE_GRAPHICS = 0,
    VULKAN_PIPELINE_TYPE_TRANSFER,
    VULKAN_PIPELINE_TYPE_COMPUTE,
};

enum pipeline_creation_state {
    VULKAN_PIPELINE_STATE_DEFAULT = 0,
    VULKAN_PIPELINE_STATE_SETUP,
    VULKAN_PIPELINE_STATE_CREATED,
    VULKAN_PIPELINE_STATE_INVALID,
};

uint32_t pipeline_get_state(uint32_t idx);

char *pipeline_get_stage_name(uint32_t idx);

uint32_t pipeline_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device);

uint32_t pipeline_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format);

uint32_t pipeline_add_vertex_input_ctx(resource_description_t *p_description);

uint32_t pipeline_start_setup(void);

uint32_t pipeline_create(VkDevice *p_device, uint32_t pipeline_idx); // TODO: add renderpass index

VkPipeline *pipeline_get_pipeline_object(uint32_t idx);

VkRenderPass *pipeline_get_renderpass_object(void);

uint32_t pipeline_show_pipeline_info(uint32_t idx);

#endif
