#ifndef __VULKAN_CMD_TYPES_H__
#define __VULKAN_CMD_TYPES_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/resource_formats.h"

#define MAX_VULKAN_SUPPORTED_CMD_TYPES      (32)
enum vulkan_supported_cmd_types {
    VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS = 0,
    VULKAN_SUPPORTED_CMD_TYPE_BIND_PIPELINE,
    VULKAN_SUPPORTED_CMD_TYPE_BIND_RESOURCE,
    VULKAN_SUPPORTED_CMD_TYPE_DRAW,
    VULKAN_SUPPORTED_CMD_TYPE_COPY_RESOURCE,
    NUM_VULKAN_SUPPORTED_CMD_TYPES,
};
#if (NUM_VULKAN_SUPPORTED_CMD_TYPES >= MAX_VULKAN_SUPPORTED_CMD_TYPES)
#error "number of command types exceeded!"
#endif

enum vulkan_subcmd_types {
    VULKAN_SUBCMD_TYPE_NONE = 0,

    // draw command
    VULKAN_SUBCMD_TYPE_DRAW_COMMAND_COMMON,
    VULKAN_SUBCMD_TYPE_DRAW_COMMAND_INDEX,
    VULKAN_SUBCMD_TYPE_DRAW_COMMAND_INDIRECT,

    NUM_VULKAN_SUBCMD_TYPES,
    INVALID_VULKAN_SUBCMD_TYPE = NUM_VULKAN_SUBCMD_TYPES,
};

typedef struct vulkan_cmd_param {
    uint32_t cmdbuf_idx;
    uint32_t subcmd_type;
    uint32_t pipeline_idx;
    uint32_t enable_semaphore;
    union {
        struct {
            VkRenderPass *p_renderpass;
            VkFramebuffer *p_framebuffer;
            VkRenderPassBeginInfo renderpass_info;
        } renderpass;

        struct {
            VkPipeline* p_pipeline;
        } bind_pipeline;

        struct {
            resource_info_t *p_resource_info;
            void *p_resource;
        } bind_resource;

        struct {
            VkRect2D scissor;
            VkViewport viewport;
            union {
                struct {
                    uint32_t vertex_count;
                } type_common;
                struct {
                    uint32_t index_count;
                } type_index;
            };

        } draw;

        struct {
            uint32_t datasize;
            union {
                VkImage image;
                VkImage buffer;
            } *p_object_src, *p_object_dst;
        } copy_resource;
    };

} vulkan_cmd_param_t;

#endif
