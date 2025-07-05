#include <stdio.h>

#include "common/common_def.h"
#include "vulkan/cmd_types.h"

#include "vulkan_cmd_pool.h"

typedef struct cmd_buf_ctx {
    uint32_t state;
    uint32_t cmd_bitmap;
    VkCommandBuffer buffer;
} cmd_buf_ctx_t;

static struct {
    uint32_t state;
    VkCommandPool pool;
    cmd_buf_ctx_t buffer_ctx[MAX_NUM_CMD_BUFFERS];
} cmd_pool_ctx;

uint32_t cmd_pool_create(VkDevice *p_device, uint32_t device_queue_idx)
{
    uint32_t res;
    VkCommandPoolCreateInfo cmd_pool_info;

    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    cmd_pool_info.queueFamilyIndex = device_queue_idx;

    res = vkCreateCommandPool(*p_device, &cmd_pool_info, NULL, &cmd_pool_ctx.pool);
    if (res != VK_SUCCESS) {
        printf("command pool create failure: %d\n", res);
    }
    else {
        cmd_pool_ctx.state = VULKAN_CMD_POOL_STATE_CREATED;
    }

    return res;
}

uint32_t cmd_pool_get_state(void)
{
    return cmd_pool_ctx.state;
}

static uint32_t __cmd_pool_get_available_buffer_idx(void)
{
    for (uint32_t idx = 0; idx < MAX_NUM_CMD_BUFFERS; idx++) {
        if (cmd_pool_get_cmd_buffer_state(idx) == VULKAN_CMD_POOL_CMDBUF_STATE_UNALLOCATED) {
            return idx;
        }
    }

    return MAX_NUM_CMD_BUFFERS;
}

static uint32_t __cmd_pool_cmdbuf_start_recording(uint32_t buf_idx)
{
    uint32_t res;
    VkCommandBuffer *p_cmd_buf;
    VkCommandBufferBeginInfo begin_info;

    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[buf_idx].buffer;

    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = NULL;
    begin_info.flags = 0;

    begin_info.pInheritanceInfo = NULL;

    res = vkBeginCommandBuffer(*p_cmd_buf, &begin_info);
    if (res != VK_SUCCESS) {
        printf("command buffer start failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t cmd_pool_allocate_buffer(VkDevice *p_device)
{
    uint32_t res;
    uint32_t buf_idx;
    cmd_buf_ctx_t *p_buf_ctx;
    VkCommandBufferAllocateInfo alloc_info;

    if (cmd_pool_get_state() != VULKAN_CMD_POOL_STATE_CREATED) {
        printf("command pool not created\n");
        return FAILURE;
    }

    buf_idx = __cmd_pool_get_available_buffer_idx();
    if (buf_idx == MAX_NUM_CMD_BUFFERS) {
        printf("buffers fully allocated\n");

        return FAILURE;
    }

    p_buf_ctx = &cmd_pool_ctx.buffer_ctx[buf_idx];

    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = NULL;

    alloc_info.commandBufferCount = 1;
    alloc_info.commandPool = cmd_pool_ctx.pool;

    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    res = vkAllocateCommandBuffers(*p_device, &alloc_info, &p_buf_ctx->buffer);
    if (res != VK_SUCCESS) {
        printf("command buffer alloc failure: %d\n", res);
        return FAILURE;
    }

    res = __cmd_pool_cmdbuf_start_recording(buf_idx);
    if (res != VK_SUCCESS) {
        printf("command buffer startup failure: %d\n", res);
        return FAILURE;
    }

    p_buf_ctx->state = VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED;

    printf("command buffer [%u] allocated\n", buf_idx);

    return SUCCESS;
}

uint32_t cmd_pool_bind_cmd_buffer_to_pipeline(uint32_t buf_idx, VkPipeline *p_pipeline)
{
    uint32_t res;
    VkCommandBuffer *p_cmd_buf;

    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[buf_idx].buffer;

    vkCmdBindPipeline(*p_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, *p_pipeline);

    vkCmdEndRenderPass(*p_cmd_buf);

    res = vkEndCommandBuffer(*p_cmd_buf);
    if (res != VK_SUCCESS) {
        printf("renderpass end failure: %d\n", res);

        return FAILURE;
    }

    return SUCCESS;
}

uint32_t cmd_pool_get_cmd_buffer_state(uint32_t buf_idx)
{
    return cmd_pool_ctx.buffer_ctx[buf_idx].state;
}

void cmd_pool_add_renderpass_command(vulkan_cmd_template_t *p_template,
                                                    vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;

    cmdbuf_idx = p_param->cmdbuf_idx;
    for (uint32_t idx = 0; idx < p_param->renderpass.framebuffer_count; idx++) {
        p_template->renderpass.framebuffer = p_param->renderpass.p_framebuffers[idx];
        vkCmdBeginRenderPass(cmd_pool_ctx.buffer_ctx[cmdbuf_idx].buffer, &p_template->renderpass,
                                                                        VK_SUBPASS_CONTENTS_INLINE);
    }

    cmd_pool_ctx.buffer_ctx[cmdbuf_idx].cmd_bitmap |= (1 << VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS);
}

void cmd_pool_add_draw_command(vulkan_cmd_template_t *p_template,
                                            vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;
    VkCommandBuffer *p_cmd_buf;

    cmdbuf_idx = p_param->cmdbuf_idx;
    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[cmdbuf_idx].buffer;

    // moved to pipeline run command
    //vkCmdBindPipeline(*p_cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, *p_param->draw.p_pipeline);

    vkCmdSetViewport(*p_cmd_buf, 0, 1, &p_param->draw.viewport);

    vkCmdSetViewport(*p_cmd_buf, 0, 1, &p_param->draw.scissor);

    // TODO: modify vertex ctx
    uint32_t vertex_count = 3;
    uint32_t instance_count = 1;
    uint32_t first_vertex = 0;
    uint32_t first_instance = 0;

    vkCmdDraw(*p_cmd_buf, vertex_count, instance_count, first_vertex, first_instance);

    cmd_pool_ctx.buffer_ctx[cmdbuf_idx].cmd_bitmap |= (1 << VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS);
}

uint32_t cmd_pool_check_buffer_allocated(uint32_t buf_idx)
{
    if (cmd_pool_get_cmd_buffer_state(buf_idx) == VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED) {
        return TRUE;
    }

    return FALSE;
}
