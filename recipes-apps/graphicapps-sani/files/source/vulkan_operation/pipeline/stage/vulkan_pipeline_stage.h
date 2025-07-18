#ifndef __VULKAN_PIPELINE_STAGE_H__
#define __VULKAN_PIPELINE_STAGE_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/pipeline_stages.h"

enum pipeline_stages_state {
    VULKAN_PIPELINE_STAGE_STATE_DEFAULT = 0,
    VULKAN_PIPELINE_STAGE_STATE_REGISTERED,
    NUM_VULKAN_PIPELINE_STAGE_STATES
};

enum dynamic_shader_state {
    VULKAN_PIPELINE_DYNAMIC_STATE_VIEWPORT,
    VULKAN_PIPELINE_DYNAMIC_STATE_SCISSOR,
    NUM_VULKAN_PIPELINE_DYNAMIC_STATES,
};

uint32_t pipeline_stage_get_status(uint32_t stage_idx);
char *pipeline_stage_get_stage_name(uint32_t stage_idx);
pipeline_stage_template_t *pipeline_stage_get_creation_info(uint32_t stage_idx);

uint32_t pipeline_stage_setup_vertex_shader_ctx(char *filename, VkDevice* p_device);
uint32_t pipeline_stage_setup_fragment_shader_ctx(char *filename, VkDevice* p_device);
uint32_t pipeline_stage_setup_viewport_ctx(VkViewport *p_viewport, VkRect2D *p_scissor);

#endif
