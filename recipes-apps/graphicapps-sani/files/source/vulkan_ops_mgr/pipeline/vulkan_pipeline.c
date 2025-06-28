#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_pipeline.h"
#include "vulkan/pipeline_stages.h"
#include "stage/vulkan_pipeline_stage.h"

static struct pipeline_ctx {
    uint32_t state;
    VkPipeline pipeline;
    VkExtent2D swapchain_extent;
    VkFormat swapchain_format;
    VkRenderPass renderpass;
    VkPipelineLayout layout;
} pipeline_ctx;

uint32_t pipeline_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device)
{
    uint32_t res;

    if (stage == VULKAN_PIPELINE_STAGE_VERTEX_SHADER) {
        res = pipeline_stage_setup_vertex_shader_ctx(filename, p_device);
    }
    else if (stage == VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER) {
        res = pipeline_stage_setup_fragment_shader_ctx(filename, p_device);
    }
    else {
        printf("invalid shader type\n");
        return FAILURE;
    }

    if (res == SUCCESS) {
        if (pipeline_ctx.state == VULKAN_PIPELINE_STATE_VIEWPORT_REGISTERED) {
            pipeline_ctx.state = VULKAN_PIPELINE_STATE_READY;
        }
    }

    return res;
}

uint32_t pipeline_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format)
{
    uint32_t res;
    VkRect2D scissor;
    VkViewport viewport;

    if (p_extent == NULL) {
        printf("viewport context is invalid\n");
        return FAILURE;
    }

    pipeline_ctx.swapchain_extent = *p_extent;
    pipeline_ctx.swapchain_format = *p_format;

    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = *p_extent;

    viewport.height = p_extent->height;
    viewport.width = p_extent->width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    res = pipeline_stage_setup_viewport_ctx(&viewport, &scissor);
    if (res == SUCCESS) {
        uint32_t stage_idx;
        stage_idx = VULKAN_PIPELINE_STAGE_VERTEX_SHADER;

        if (pipeline_stage_get_status(stage_idx) == VULKAN_PIPELINE_STAGE_STATE_REGISTERED) {
            pipeline_ctx.state = VULKAN_PIPELINE_STATE_READY;
        }
        else {
            pipeline_ctx.state = VULKAN_PIPELINE_STATE_VIEWPORT_REGISTERED;
        }
    }

    return res;
}

uint32_t pipeline_get_creation_state(uint32_t idx)
{
    return pipeline_ctx.state;
}

static uint32_t __pipeline_create_layout(VkDevice *p_device)
{
    VkPipelineLayoutCreateInfo layout_info;

    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pNext = NULL;
    layout_info.flags = 0;
    layout_info.setLayoutCount = 0;
    layout_info.pSetLayouts = NULL;
    layout_info.pushConstantRangeCount = 0;
    layout_info.pPushConstantRanges = NULL;

    return vkCreatePipelineLayout(*p_device, &layout_info, NULL, &pipeline_ctx.layout);
}

static uint32_t __pipeline_create_renderpass(VkDevice *p_device, VkFormat *p_format)
{
    VkAttachmentDescription color_attachment;
    color_attachment.format = *p_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo renderpass_info;
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_info.flags = 0;
    renderpass_info.attachmentCount = 1;
    renderpass_info.pAttachments = &color_attachment;
    renderpass_info.subpassCount = 1;
    renderpass_info.pSubpasses = &subpass;

    return vkCreateRenderPass(*p_device, &renderpass_info, NULL, &pipeline_ctx.renderpass);
}

static uint32_t __pipeline_create(VkDevice *p_device)
{
    stage_creation_info_t *p_stage_info;
    VkGraphicsPipelineCreateInfo pipeline_info;
    VkPipelineShaderStageCreateInfo p_shader_stages[2];

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_VERTEX_SHADER);
    p_shader_stages[0] = p_stage_info->vertex_shader;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER);
    p_shader_stages[1] = p_stage_info->fragment_shader;

    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = NULL;
    pipeline_info.flags = 0;

    pipeline_info.stageCount = 2;
    pipeline_info.pStages = p_shader_stages;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_VERTEX_INPUT);
    pipeline_info.pVertexInputState = &p_stage_info->vertex_input;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_INPUT_ASSEMBLY);
    pipeline_info.pInputAssemblyState = &p_stage_info->input_assembly;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_VIEWPORT);
    pipeline_info.pViewportState = &p_stage_info->viewport;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_RASTERIZER);
    pipeline_info.pRasterizationState = &p_stage_info->rasterizer;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_MULTISAMPLING);
    pipeline_info.pMultisampleState = &p_stage_info->multisampling;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_DEPTH_STENCIL);
    pipeline_info.pDepthStencilState = NULL;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_COLOR_BLENDING);
    pipeline_info.pColorBlendState = &p_stage_info->color_blend;

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_DYNAMIC);
    pipeline_info.pDynamicState = &p_stage_info->dynamic;

    pipeline_info.layout = pipeline_ctx.layout;
    pipeline_info.renderPass = pipeline_ctx.renderpass;
    pipeline_info.subpass = 0;

    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    return vkCreateGraphicsPipelines(*p_device, VK_NULL_HANDLE, 1, &pipeline_info,
                                                    NULL, &pipeline_ctx.pipeline);;
}


uint32_t pipeline_create(VkDevice *p_device)
{
    uint32_t res;
    uint32_t stage_idx;

    stage_idx = VULKAN_PIPELINE_STAGE_VERTEX_SHADER;
    if (pipeline_stage_get_status(stage_idx) != VULKAN_PIPELINE_STAGE_STATE_REGISTERED) {
        printf("vertex shader is not registered\n");
        return FAILURE;
    }

    res = __pipeline_create_renderpass(p_device, &pipeline_ctx.swapchain_format);
    if (res != VK_SUCCESS) {
        printf("renderpass create failed: %d\n", res);
        return FAILURE;
    }

    res = __pipeline_create_layout(p_device);
    if (res != VK_SUCCESS) {
        printf("pipeline layout create failed: %d\n", res);
        return FAILURE;
    }

    res = __pipeline_create(p_device);
    if (res != VK_SUCCESS) {
        printf("pipeline create failure: %d\n", res);
        return FAILURE;
    }

    pipeline_ctx.state = VULKAN_PIPELINE_STATE_CREATED;

    return SUCCESS;
}

uint32_t pipeline_show_pipeline_info(uint32_t stage)
{

}