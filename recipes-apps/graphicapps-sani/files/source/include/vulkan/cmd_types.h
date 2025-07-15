#ifndef __VULKAN_CMD_TYPES_H__
#define __VULKAN_CMD_TYPES_H__

#include <vulkan/vulkan_core.h>

#define MAX_VULKAN_SUPPORTED_CMD_TYPES      (31)
enum vulkan_supported_cmd_types {
    VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS = 0,
    VULKAN_SUPPORTED_CMD_TYPE_DRAW,
    NUM_VULKAN_SUPPORTED_CMD_TYPES,
};

#if (NUM_VULKAN_SUPPORTED_CMD_TYPES > MAX_VULKAN_SUPPORTED_CMD_TYPES)
#error "number of command types exceeded!"
#endif

typedef union vulkan_cmd_template {
    VkRenderPassBeginInfo renderpass;
} vulkan_cmd_template_t;

typedef struct vulkan_cmd_param {
    uint32_t cmdbuf_idx;
    uint32_t enable_semaphore;
    union {
        struct {
            uint32_t clear_value_count;
            uint32_t framebuffer_count;
            uint32_t framebuffer_image_idx;

            VkRenderPass *p_renderpass;
            VkExtent2D *p_swapchain_extent;

            VkFramebuffer *p_framebuffers;
            VkClearValue *p_clear_values;
        } renderpass;

        struct {
            VkRect2D scissor;
            VkViewport viewport;
            VkPipeline* p_pipeline;
        } draw;
    };

} vulkan_cmd_param_t;

#endif
