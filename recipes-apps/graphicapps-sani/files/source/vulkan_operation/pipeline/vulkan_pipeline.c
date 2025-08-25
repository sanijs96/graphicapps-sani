#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan/pipeline_stages.h"
#include "vulkan/resource_formats.h"
#include "stage/vulkan_pipeline_stage.h"

#include "vulkan_pipeline.h"

typedef struct renderpass_ctx {
    VkAttachmentDescription color_attachment;
    VkAttachmentReference color_attachment_ref;
    VkSubpassDescription subpass;
    VkSubpassDependency dependency;
    VkRenderPassCreateInfo renderpass_info;
    VkRenderPass renderpass;
} renderpass_ctx_t;

// TODO: support multiple pipelines
static struct {
    uint32_t states[MAX_NUM_VULKAN_PIPELINES];
    VkPipeline pipelines[MAX_NUM_VULKAN_PIPELINES];
    VkPipelineLayout layout;

    VkFormat swapchain_format;
    VkExtent2D swapchain_extent;

    renderpass_ctx_t renderpass_ctx;
    struct {
        uint32_t binding_count;
        uint32_t attribute_count;

        // (TODO) might be exceeded
        VkVertexInputBindingDescription bindings[MAX_NUM_RESOURCE_OBJECTS];
        VkVertexInputAttributeDescription attributes[MAX_NUM_RESOURCE_OBJECTS];
    } vertex_input;
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

    return res;
}

uint32_t pipeline_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format)
{
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

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)p_extent->width;
    viewport.height = (float)p_extent->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    if (pipeline_stage_setup_viewport_ctx(&viewport, &scissor) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t pipeline_add_vertex_input_ctx(resource_description_t *p_description)
{
    return pipeline_stage_setup_vertex_input_ctx(p_description);
}

uint32_t pipeline_get_state(uint32_t idx)
{
    return pipeline_ctx.states[idx];
}

char *pipeline_get_stage_name(uint32_t idx)
{
    return pipeline_stage_get_stage_name(idx);
}

static uint32_t __pipeline_get_available_entry(void)
{
    for (uint32_t pipeline_idx = 0; pipeline_idx < MAX_NUM_VULKAN_PIPELINES; pipeline_idx++) {
        if (pipeline_get_state(pipeline_idx) == VULKAN_PIPELINE_STATE_DEFAULT) {
            return pipeline_idx;
        }
    }

    return MAX_NUM_VULKAN_PIPELINES;
}

uint32_t pipeline_start_setup(void)
{
    uint32_t pipeline_entry_idx;

    pipeline_entry_idx = __pipeline_get_available_entry();

    if (pipeline_entry_idx == MAX_NUM_VULKAN_PIPELINES) {
        printf("no pipeline entries available\n");

        return FAILURE;
    }

    pipeline_ctx.states[pipeline_entry_idx] = VULKAN_PIPELINE_STATE_SETUP;

    return SUCCESS;
}

static uint32_t __pipeline_create_layout(VkDevice *p_device)
{
    uint32_t res;
    VkPipelineLayoutCreateInfo layout_info;

    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pNext = NULL;
    layout_info.flags = 0;

    layout_info.pSetLayouts = NULL;
    layout_info.setLayoutCount = 0;

    layout_info.pPushConstantRanges = NULL;
    layout_info.pushConstantRangeCount = 0;

    res = vkCreatePipelineLayout(*p_device, &layout_info, NULL, &pipeline_ctx.layout);
    if (res != VK_SUCCESS) {
        printf("pipeline layout create failed: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __pipeline_create_renderpass(VkDevice *p_device)
{
    uint32_t res;
    renderpass_ctx_t *p_ctx;

    p_ctx = &pipeline_ctx.renderpass_ctx;

    p_ctx->color_attachment.flags = 0;
    p_ctx->color_attachment.format = pipeline_ctx.swapchain_format;
    p_ctx->color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    p_ctx->color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    p_ctx->color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    p_ctx->color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    p_ctx->color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    p_ctx->color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // don't care since cleared on load operation
    p_ctx->color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // image layout for presentation through swap chain

    p_ctx->color_attachment_ref.attachment = 0; // attachment index in renderpass create info
    p_ctx->color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    p_ctx->subpass.flags = 0;
    p_ctx->subpass.pColorAttachments = &p_ctx->color_attachment_ref;
    p_ctx->subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    p_ctx->subpass.colorAttachmentCount = 1;
    p_ctx->subpass.inputAttachmentCount = 0;
    p_ctx->subpass.preserveAttachmentCount = 0;

    // starting point for all subpasses
    p_ctx->dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // finish event is not from subpass
    p_ctx->dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait until swapchain reads from the image
    p_ctx->dependency.srcAccessMask = 0;

    // finish point on subpass[0]
    p_ctx->dependency.dstSubpass = 0; // must be always higher than srcSubpass
    p_ctx->dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // finishes on color attachment output stage
    p_ctx->dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // write color attachment
    p_ctx->dependency.dependencyFlags = 0;

    p_ctx->renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    p_ctx->renderpass_info.flags = 0;
    p_ctx->renderpass_info.attachmentCount = 1;
    p_ctx->renderpass_info.pAttachments = &p_ctx->color_attachment;
    p_ctx->renderpass_info.subpassCount = 1;
    p_ctx->renderpass_info.pSubpasses = &p_ctx->subpass;
    p_ctx->renderpass_info.dependencyCount = 1;
    p_ctx->renderpass_info.pDependencies = &p_ctx->dependency;

    //TODO: 분리
    res = vkCreateRenderPass(*p_device, &p_ctx->renderpass_info, NULL, &p_ctx->renderpass);
    if (res != VK_SUCCESS) {
        printf("renderpass create failed: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __pipeline_create_pipeline(VkDevice *p_device, uint32_t pipeline_idx)
{
    uint32_t res;
    pipeline_stage_template_t *p_stage_info;
    VkGraphicsPipelineCreateInfo pipeline_info;
    VkPipelineShaderStageCreateInfo p_shader_stages[2];

    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = NULL;
    pipeline_info.flags = 0;

    pipeline_info.stageCount = 0;
    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_VERTEX_SHADER);
    if (p_stage_info->vertex_shader.pName != NULL) {
        p_shader_stages[0] = p_stage_info->vertex_shader;
        pipeline_info.stageCount++;
    }

    p_stage_info = pipeline_stage_get_creation_info(VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER);
    if (p_stage_info->fragment_shader.pName != NULL) {
        p_shader_stages[1] = p_stage_info->fragment_shader;
        pipeline_info.stageCount++;
    }

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
    pipeline_info.renderPass = pipeline_ctx.renderpass_ctx.renderpass;

    // subpass index where this graphics pipeline will be used
    // if using other subpass, another pipeline should be created (renderpass should be compatible)
    pipeline_info.subpass = 0;

    // previous pipeline object, if exist
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    res = vkCreateGraphicsPipelines(*p_device, VK_NULL_HANDLE, 1, &pipeline_info,
                                                    NULL, &pipeline_ctx.pipelines[pipeline_idx]);
    if (res != VK_SUCCESS) {
        printf("pipeline create failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __pipeline_get_setup_entry(void)
{
    uint32_t pipeline_entry_idx;

    pipeline_entry_idx = MAX_NUM_VULKAN_PIPELINES;

    for (uint32_t idx = 0; idx < MAX_NUM_VULKAN_PIPELINES; idx++) {
        if (pipeline_get_state(idx) != VULKAN_PIPELINE_STATE_SETUP) {
            continue;
        }

        pipeline_entry_idx = idx;
    }

    return pipeline_entry_idx;
}

uint32_t pipeline_create(VkDevice *p_device, uint32_t pipeline_entry_idx)
{
    uint32_t stage_idx;

    stage_idx = VULKAN_PIPELINE_STAGE_VERTEX_SHADER;

    if (pipeline_stage_get_status(stage_idx) != VULKAN_PIPELINE_STAGE_STATE_REGISTERED) {
        printf("vertex shader is not registered\n");
        return FAILURE;
    }

    // TODO: add dedicated handler
    if (__pipeline_create_renderpass(p_device) == FAILURE) {
        return FAILURE;
    }

    if (__pipeline_create_layout(p_device) == FAILURE) {
        return FAILURE;
    }

    if (__pipeline_create_pipeline(p_device, pipeline_entry_idx) == FAILURE) {
        return FAILURE;
    }

    pipeline_ctx.states[pipeline_entry_idx] = VULKAN_PIPELINE_STATE_CREATED;

    return SUCCESS;
}

uint32_t pipeline_destroy(VkDevice *p_device, uint32_t idx)
{
    if (pipeline_ctx.states[idx] != VULKAN_PIPELINE_STATE_CREATED) {
        printf("pipeline entry is not created\n");
        return FAILURE;
    }

    vkDestroyPipeline(*p_device, pipeline_ctx.pipelines[idx], NULL);

    pipeline_ctx.states[idx] = VULKAN_PIPELINE_STATE_DEFAULT;

    return SUCCESS;
}

VkPipeline *pipeline_get_pipeline_object(uint32_t idx)
{
    return &pipeline_ctx.pipelines[idx];
}

VkRenderPass *pipeline_get_renderpass_object(void)
{
    return &pipeline_ctx.renderpass_ctx.renderpass;
}

uint32_t pipeline_show_pipeline_info(uint32_t idx)
{

}
