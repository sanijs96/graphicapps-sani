#include <stdio.h>

#include "common/common_def.h"
#include "vulkan/cmd_types.h"

#include "vulkan_ops_mgr.h"

#include "pipeline/vulkan_pipeline.h"
#include "command/vulkan_cmd_pool.h"
#include "command/vulkan_cmd_template.h"

typedef struct vulkan_cmd_proc_ctx{
    uint32_t (*setup)(vulkan_cmd_template_t *p_template, vulkan_cmd_param_t *p_param);
    void (*add)(vulkan_cmd_template_t *p_template, vulkan_cmd_param_t *p_param);
} vulkan_cmd_proc_ctx_t;

static vulkan_cmd_proc_ctx_t cmd_proc_ctx[NUM_VULKAN_SUPPORTED_CMD_TYPES] =
{
    [VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS] = {
        .setup = cmd_template_setup_renderpass_command,
        .add = cmd_pool_add_renderpass_command,
    },
    [VULKAN_SUPPORTED_CMD_TYPE_DRAW] = {
        .setup = NULL,
        .add = cmd_pool_add_draw_command,
    },
};

uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device)
{
    return pipeline_add_shader_file(stage, filename, p_device);
}

uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format)
{
    return pipeline_add_viewport_ctx(p_extent, p_format);
}

uint32_t vulkan_ops_mgr_create_pipeline(VkDevice *p_device)
{
    return pipeline_create(p_device);
}

uint32_t vulkan_ops_mgr_run_pipeline(VkDevice *p_device)
{
    return FAILURE;
}

uint32_t vulkan_ops_mgr_check_pipeline_created(void)
{
    if (pipeline_get_creation_state() == VULKAN_PIPELINE_STATE_CREATED) {
        return TRUE;
    }

    return FALSE;
}

VkPipeline *vulkan_ops_mgr_get_pipeline_object(void)
{
    if (pipeline_get_creation_state() == VULKAN_PIPELINE_STATE_CREATED) {
        return pipeline_get_pipeline_object();
    }

    return NULL;
}

VkRenderPass *vulkan_ops_mgr_get_renderpass_object(void)
{
    return pipeline_get_renderpass_object();
}

uint32_t vulkan_ops_mgr_check_cmd_buffer_allocated(uint32_t buf_idx)
{
    if (cmd_pool_get_cmd_buffer_state(buf_idx) == VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED) {
        return TRUE;
    }

    return FALSE;
}

uint32_t vulkan_ops_mgr_allocate_cmd_buffer(VkDevice *p_device, uint32_t graphics_queue_idx)
{
    uint32_t res;

    if (cmd_pool_get_state() != VULKAN_CMD_POOL_STATE_CREATED) {
        res = cmd_pool_create(p_device, graphics_queue_idx);
        if (res == FAILURE) {
            return res;
        }
    }

    res = cmd_pool_allocate_buffer(p_device);

    return res;
}

uint32_t vulkan_ops_mgr_activate_cmd_buffer(uint32_t cmdbuf_idx, VkPipeline *p_pipeline)
{
    return cmd_pool_bind_cmd_buffer_to_pipeline(cmdbuf_idx, p_pipeline);
}

uint32_t vulkan_ops_mgr_add_vulkan_command(uint32_t cmd_type, vulkan_cmd_param_t *p_param)
{
    uint32_t res;
    vulkan_cmd_template_t template;

    if (cmd_proc_ctx[cmd_type].setup) {
        res = cmd_proc_ctx[cmd_type].setup(&template, p_param);

        if (res == FAILURE) {
            printf("command template setup failure\n");
            return FAILURE;
        }
    }

    if (!cmd_proc_ctx[cmd_type].add) {
        printf("command handler not implemented\n");
        return FAILURE;
    }

    cmd_proc_ctx->add(&template, p_param);

    return SUCCESS;
}
