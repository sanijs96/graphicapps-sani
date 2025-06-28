#ifndef __VULKAN_PIPELINE_STAGES_H__
#define __VULKAN_PIPELINE_STAGES_H__

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

typedef struct stage_creation_info {
    union {
        VkPipelineShaderStageCreateInfo vertex_shader;
        VkPipelineShaderStageCreateInfo fragment_shader;
        VkPipelineVertexInputStateCreateInfo vertex_input;
        VkPipelineInputAssemblyStateCreateInfo input_assembly;
        VkPipelineViewportStateCreateInfo viewport;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineDepthStencilStateCreateInfo depth_stencil;
        VkPipelineColorBlendStateCreateInfo color_blend;
        VkPipelineDynamicStateCreateInfo dynamic;
    };
} stage_creation_info_t;

uint32_t pipeline_stage_get_status(uint32_t stage_idx);
stage_creation_info_t *pipeline_stage_get_creation_info(uint32_t stage_idx);

uint32_t pipeline_stage_setup_vertex_shader_ctx(char *filename, VkDevice* p_device);
uint32_t pipeline_stage_setup_fragment_shader_ctx(char *filename, VkDevice* p_device);
uint32_t pipeline_stage_setup_viewport_ctx(VkViewport *p_viewport, VkRect2D *p_scissor);

#endif
