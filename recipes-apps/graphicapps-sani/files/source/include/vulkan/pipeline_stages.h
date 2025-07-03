#ifndef __VULKAN_PIPELINE_STAGES_H__
#define __VULKAN_PIPELINE_STAGES_H__

#include <vulkan/vulkan_core.h>

enum pipeline_stages {
    VULKAN_PIPELINE_STAGE_VERTEX_SHADER = 0,
    VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER,
    VULKAN_PIPELINE_STAGE_VERTEX_INPUT,
    VULKAN_PIPELINE_STAGE_INPUT_ASSEMBLY,
    VULKAN_PIPELINE_STAGE_VIEWPORT,
    VULKAN_PIPELINE_STAGE_DYNAMIC,
    VULKAN_PIPELINE_STAGE_RASTERIZER,
    VULKAN_PIPELINE_STAGE_MULTISAMPLING,
    VULKAN_PIPELINE_STAGE_DEPTH_STENCIL,
    VULKAN_PIPELINE_STAGE_COLOR_BLENDING,
    NUM_VULKAN_PIPELINE_STAGES
};

typedef union pipeline_stage_template {
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
} pipeline_stage_template_t;

#endif