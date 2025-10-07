#include <stdio.h>

#include "common/common_def.h"
#include "vulkan/common_def.h"
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

uint32_t cmd_pool_create(VkDevice *p_device, uint32_t queue_family_idx)
{
    uint32_t res;
    VkCommandPoolCreateInfo cmd_pool_info;

    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    cmd_pool_info.queueFamilyIndex = queue_family_idx;

    res = vkCreateCommandPool(*p_device, &cmd_pool_info, NULL, &cmd_pool_ctx.pool);
    if (res != VK_SUCCESS) {
        printf("command pool create failure: %d\n", res);
    }
    else {
        cmd_pool_ctx.state = VULKAN_CMD_POOL_STATE_CREATED;
    }

    for (uint32_t idx = 0; idx < MAX_NUM_CMD_BUFFERS; idx++) {
        cmd_pool_ctx.buffer_ctx[idx].state = VULKAN_CMD_POOL_CMDBUF_STATE_DEFAULT;
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
        if (cmd_pool_get_cmd_buffer_state(idx) == VULKAN_CMD_POOL_CMDBUF_STATE_DEFAULT) {
            return idx;
        }
    }

    return MAX_NUM_CMD_BUFFERS;
}

static uint32_t __cmd_pool_setup_cmd_buffer_type_flag(uint32_t buf_type)
{
    uint32_t buf_type_flag;

    switch (buf_type) {
        case VULKAN_CMD_POOL_CMDBUF_TYPE_COMMON:
            buf_type_flag = 0;
            break;

        case VULKAN_CMD_POOL_CMDBUF_TYPE_ONETIME:
            buf_type_flag = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            break;

        default:
            printf("invalid buffer type\n");
            buf_type_flag = UINT32_MAX;
            break;
    }

    return buf_type_flag;
}

static uint32_t __cmd_pool_cmdbuf_start_recording(uint32_t buf_idx, uint32_t buf_type)
{
    uint32_t res;
    VkCommandBuffer *p_cmd_buf;
    VkCommandBufferBeginInfo begin_info;

    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[buf_idx].buffer;

    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = NULL;

    begin_info.pInheritanceInfo = NULL;

    begin_info.flags = __cmd_pool_setup_cmd_buffer_type_flag(buf_type);

    vkResetCommandBuffer(*p_cmd_buf, 0);

    res = vkBeginCommandBuffer(*p_cmd_buf, &begin_info);
    if (res != VK_SUCCESS) {
        printf("command buffer start failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t cmd_pool_allocate_buffer(VkDevice *p_device, uint32_t buf_type)
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

    res = __cmd_pool_cmdbuf_start_recording(buf_idx, buf_type);
    if (res != VK_SUCCESS) {
        printf("command buffer startup failure: %d\n", res);
        return FAILURE;
    }

    p_buf_ctx->state = VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED;

    printf("command buffer [%u] allocated\n", buf_idx);

    return buf_idx;
}

uint32_t cmd_pool_free_cmd_buffer(VkDevice *p_device, uint32_t buf_idx)
{
    cmd_buf_ctx_t *p_buf_ctx;

    p_buf_ctx = &cmd_pool_ctx.buffer_ctx[buf_idx];

    vkFreeCommandBuffers(*p_device, cmd_pool_ctx.pool, 1, &p_buf_ctx->buffer);

    p_buf_ctx->state = VULKAN_CMD_POOL_CMDBUF_STATE_DEFAULT;

    return SUCCESS;
}

uint32_t cmd_pool_finish_buffer_recording(uint32_t buf_idx)
{
    uint32_t res;
    uint32_t cmd_buf_bitmap;
    VkCommandBuffer *p_cmd_buf;

    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[buf_idx].buffer;

    cmd_buf_bitmap = cmd_pool_ctx.buffer_ctx[buf_idx].cmd_bitmap;

    if (cmd_buf_bitmap & (1 << VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS)) {
        vkCmdEndRenderPass(*p_cmd_buf);
    }

    res = vkEndCommandBuffer(*p_cmd_buf);
    if (res != VK_SUCCESS) {
        printf("renderpass end failure: %d\n", res);

        return FAILURE;
    }

    cmd_pool_ctx.buffer_ctx[buf_idx].state = VULKAN_CMD_POOL_CMDBUF_STATE_ACTIVATED;

    return SUCCESS;
}

uint32_t cmd_pool_get_cmd_buffer_state(uint32_t buf_idx)
{
    return cmd_pool_ctx.buffer_ctx[buf_idx].state;
}

uint32_t cmd_pool_get_cmd_buffer_bitmap(uint32_t buf_idx)
{
    return cmd_pool_ctx.buffer_ctx[buf_idx].cmd_bitmap;
}

VkCommandBuffer *cmd_pool_get_cmd_buffer_object(uint32_t buf_idx)
{
    return &cmd_pool_ctx.buffer_ctx[buf_idx].buffer;
}

void cmd_pool_add_renderpass_command(vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;
    VkCommandBuffer *p_cmd_buf;

    cmdbuf_idx = p_param->cmdbuf_idx;
    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[cmdbuf_idx].buffer;

    vkCmdBeginRenderPass(*p_cmd_buf, &p_param->renderpass.renderpass_info,
                                                VK_SUBPASS_CONTENTS_INLINE);

    cmd_pool_ctx.buffer_ctx[cmdbuf_idx].cmd_bitmap |= (1 << VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS);
}

void cmd_pool_add_bind_pipeline_command(vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;
    cmd_buf_ctx_t *p_buf_ctx;

    cmdbuf_idx = p_param->cmdbuf_idx;
    p_buf_ctx = &cmd_pool_ctx.buffer_ctx[cmdbuf_idx];

    vkCmdBindPipeline(p_buf_ctx->buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        *p_param->bind_pipeline.p_pipeline);

    p_buf_ctx->cmd_bitmap |= (1 << VULKAN_SUPPORTED_CMD_TYPE_BIND_PIPELINE);
}

void cmd_pool_add_bind_resource_command(vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;
    resource_info_t *p_info;
    cmd_buf_ctx_t *p_buf_ctx;
    const VkDeviceSize offset[] = {0};

    cmdbuf_idx = p_param->cmdbuf_idx;

    p_info = p_param->bind_resource.p_resource_info;

    p_buf_ctx = &cmd_pool_ctx.buffer_ctx[cmdbuf_idx];

    if (p_info->type >= RESOURCE_FORMAT_TYPE_IMAGE_START) {
        // TODO
    }
    else if (p_info->type >= RESOURCE_FORMAT_TYPE_INDEX_BUFFER_START) {
        vkCmdBindIndexBuffer(p_buf_ctx->buffer,
                                *(VkBuffer *)p_param->bind_resource.p_resource,
                                            0, p_info->index_buffer.index_type);
    }
    else {
        vkCmdBindVertexBuffers(p_buf_ctx->buffer,
                                // binding idx & count, associated with vertex input stage
                                p_info->vertex_buffer.binding_idx, 1,
                                p_param->bind_resource.p_resource, offset);
    }

    p_buf_ctx->cmd_bitmap |= (1 << VULKAN_SUPPORTED_CMD_TYPE_BIND_RESOURCE);
}

void __cmd_pool_add_draw_command(vulkan_cmd_param_t *p_param, VkCommandBuffer *p_cmd_buf)
{
    uint32_t subcmd_type;
    uint32_t instance_count;
    uint32_t first_instance;

    subcmd_type = p_param->subcmd_type;

    // no instancing(TBD)
    instance_count = 1;
    first_instance = 0;

    if (subcmd_type == VULKAN_SUBCMD_TYPE_DRAW_COMMAND_COMMON) {
        uint32_t first_vertex = 0;

        vkCmdDraw(*p_cmd_buf, p_param->draw.type_common.vertex_count,
                        instance_count, first_vertex, first_instance);
    }
    else if (subcmd_type == VULKAN_SUBCMD_TYPE_DRAW_COMMAND_INDEX) {
        uint32_t first_index = 0;
        uint32_t vertex_offset = 0;

        vkCmdDrawIndexed(*p_cmd_buf, p_param->draw.type_index.index_count,
                                                instance_count, first_index,
                                                vertex_offset, first_instance);
    }
    else {
        printf("unsupported draw command type\n");

        return FAILURE;
    }

    return SUCCESS;
}

void cmd_pool_add_draw_command(vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;
    VkCommandBuffer *p_cmd_buf;

    cmdbuf_idx = p_param->cmdbuf_idx;
    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[cmdbuf_idx].buffer;

    vkCmdSetViewport(*p_cmd_buf, 0, 1, &p_param->draw.viewport);

    vkCmdSetScissor(*p_cmd_buf, 0, 1, &p_param->draw.scissor);

    __cmd_pool_add_draw_command(p_param, p_cmd_buf);

    cmd_pool_ctx.buffer_ctx[cmdbuf_idx].cmd_bitmap |= (1 << VULKAN_SUPPORTED_CMD_TYPE_DRAW);
}

void cmd_pool_add_copy_resource_command(vulkan_cmd_param_t *p_param)
{
    uint32_t cmdbuf_idx;
    VkBufferCopy copy_region;
    VkCommandBuffer *p_cmd_buf;

    cmdbuf_idx = p_param->cmdbuf_idx;
    p_cmd_buf = &cmd_pool_ctx.buffer_ctx[cmdbuf_idx].buffer;

    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;

    copy_region.size = p_param->copy_resource.datasize;

    vkCmdCopyBuffer(*p_cmd_buf, p_param->copy_resource.p_object_src->buffer,
                    p_param->copy_resource.p_object_dst->buffer, 1, &copy_region);

    cmd_pool_ctx.buffer_ctx[cmdbuf_idx].cmd_bitmap |=
                    (1 << VULKAN_SUPPORTED_CMD_TYPE_COPY_RESOURCE);
}

uint32_t cmd_pool_check_buffer_allocated(uint32_t buf_idx)
{
    if (cmd_pool_get_cmd_buffer_state(buf_idx) == VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED) {
        return TRUE;
    }

    return FALSE;
}
