#include <stdio.h>

#include "common/common_def.h"

#include "vulkan_cmd_pool.h"

enum buffer_creation_state {
    VULKAN_CMD_POOL_BUFFER_STATE_UNALLOCATED,
    VULKAN_CMD_POOL_BUFFER_STATE_ALLOCATED,
    NUM_VULKAN_CMD_POOL_BUFFER_STATES,
};

typedef struct cmd_buf_ctx {
    uint32_t state;
    VkCommandBuffer buffer;
} cmd_buf_ctx_t;

static struct cmd_pool_ctx {
    uint32_t state;
    VkCommandPool pool;
    cmd_buf_ctx_t buffer_ctx[MAX_NUM_CMD_BUFFERS];
} cmd_pool_ctx;

uint32_t vulkan_cmd_pool_create(VkDevice *p_device, uint32_t device_queue_idx)
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

uint32_t vulkan_cmd_pool_get_state(void)
{
    return cmd_pool_ctx.state;
}

static uint32_t vulkan_cmd_pool_start_cmd_buf_recording(VkCommandBuffer *p_cmd_buf)
{
    uint32_t res;
    VkCommandBufferBeginInfo begin_info;

    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = NULL;
    begin_info.flags = 0;

    begin_info.pInheritanceInfo = NULL;

    res = vkBeginCommandBuffer(*p_cmd_buf, &begin_info);

    return res;
}

static uint32_t __vulkan_cmd_pool_get_available_buffer_idx(void)
{
    cmd_buf_ctx_t *p_buf_ctx;
    for (uint32_t idx = 0; idx < MAX_NUM_CMD_BUFFERS; idx++) {
        p_buf_ctx = &cmd_pool_ctx.buffer_ctx[idx];

        if (p_buf_ctx->state == VULKAN_CMD_POOL_BUFFER_STATE_UNALLOCATED) {
            return idx;
        }
    }

    return MAX_NUM_CMD_BUFFERS;
}

uint32_t vulkan_cmd_pool_allocate_buffer(VkDevice *p_device)
{
    uint32_t res;
    uint32_t buf_idx;
    cmd_buf_ctx_t *p_buf_ctx;
    VkCommandBufferAllocateInfo alloc_info;

    if (vulkan_cmd_pool_get_state() != VULKAN_CMD_POOL_STATE_CREATED) {
        printf("command pool not created\n");
        return NULL;
    }

    buf_idx = __vulkan_cmd_pool_get_available_buffer_idx();
    if (buf_idx == MAX_NUM_CMD_BUFFERS) {
        printf("buffers fully allocated\n");
        return NULL;
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

    res = vulkan_cmd_pool_start_cmd_buf_recording(&p_buf_ctx->buffer);
    if (res != VK_SUCCESS) {
        printf("command buffer startup failure: %d\n", res);
        return FAILURE;
    }

    p_buf_ctx->state = VULKAN_CMD_POOL_BUFFER_STATE_ALLOCATED;

    return SUCCESS;
}
