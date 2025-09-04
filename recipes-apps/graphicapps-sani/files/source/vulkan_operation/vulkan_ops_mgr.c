#include <stdio.h>
#include <string.h>

#include "common/common_def.h"
#include "vulkan/common_def.h"

#include "vulkan/cmd_types.h"
#include "vulkan/pipeline_stages.h"

#include "vulkan_ops_mgr.h"

#include "pipeline/vulkan_pipeline.h"
#include "command/vulkan_cmd_pool.h"

#define MAX_VULKAN_CMDNAME_LENGTH       (20)

typedef struct vulkan_cmd_proc_ctx{
    const char cmdname[MAX_VULKAN_CMDNAME_LENGTH];
    //uint32_t (*setup)(vulkan_cmd_template_t *p_template, vulkan_cmd_param_t *p_param);
    void (*add)(vulkan_cmd_param_t *p_param);
} vulkan_cmd_proc_ctx_t;

const vulkan_cmd_proc_ctx_t cmd_proc_ctx[NUM_VULKAN_SUPPORTED_CMD_TYPES] =
{
    [VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS] = {
        .cmdname = "begin_renderpass",
        //.setup = cmd_setup_renderpass_command,
        .add = cmd_pool_add_renderpass_command,
    },
    [VULKAN_SUPPORTED_CMD_TYPE_BIND_PIPELINE] = {
        .cmdname = "bind_pipeline",
        //.setup = NULL,
        .add = cmd_pool_add_bind_pipeline_command,
    },
    [VULKAN_SUPPORTED_CMD_TYPE_BIND_RESOURCE] = {
        .cmdname = "bind_resource",
        //.setup = NULL,
        .add = cmd_pool_add_bind_resource_command,
    },
    [VULKAN_SUPPORTED_CMD_TYPE_DRAW] = {
        .cmdname = "draw",
        //.setup = NULL,
        .add = cmd_pool_add_draw_command,
    },
    [VULKAN_SUPPORTED_CMD_TYPE_COPY_RESOURCE] = {
        .cmdname = "copy_resource",
        //.setup = NULL,
        .add = cmd_pool_add_copy_resource_command,
    },
};

uint32_t vulkan_ops_mgr_get_vulkan_cmd_buffer_type(const char *p_type_name)
{
    char *type_names_ref[NUM_VULKAN_CMD_POOL_CMDBUF_TYPES] = {
	[VULKAN_CMD_POOL_CMDBUF_TYPE_COMMON] = "common",
	[VULKAN_CMD_POOL_CMDBUF_TYPE_ONETIME] = "onetime"
    };

    for (uint32_t idx = 0; idx < NUM_VULKAN_CMD_POOL_CMDBUF_TYPES; idx++) {
        if (!strncmp(p_type_name, type_names_ref[idx], strlen(type_names_ref[idx]))) {
            printf("type name %s, idx %u\n", p_type_name, idx);

            return idx;
        }
    }

    printf("undefined command buffer type: %s\n", p_type_name);

    return NUM_VULKAN_CMD_POOL_CMDBUF_TYPES;
}

uint32_t vulkan_ops_mgr_get_vulkan_cmd_type_from_name(const char *p_cmd_name)
{
    for (uint32_t idx = 0; idx < NUM_VULKAN_SUPPORTED_CMD_TYPES; idx++) {
        if (strcmp(cmd_proc_ctx[idx].cmdname, p_cmd_name)) {
            continue;
        }

        return idx;
    }

    return NUM_VULKAN_SUPPORTED_CMD_TYPES;
}

uint32_t vulkan_ops_mgr_get_pipeline_stage_idx(const char *stage_name_str)
{
    const char *stage_name_str_ref;
    for (uint32_t stage_idx = 0; stage_idx < NUM_VULKAN_PIPELINE_STAGES; stage_idx++) {
        stage_name_str_ref = pipeline_get_stage_name(stage_idx);

        if (!strcmp(stage_name_str, stage_name_str_ref)) {
            return stage_idx;
        }
    }

    printf("pipeline stage index invalid\n");

    return NUM_VULKAN_PIPELINE_STAGES;
}

uint32_t vulkan_ops_mgr_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device)
{
    return pipeline_add_shader_file(stage, filename, p_device);
}

uint32_t vulkan_ops_mgr_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format)
{
    return pipeline_add_viewport_ctx(p_extent, p_format);
}

uint32_t vulkan_ops_mgr_add_vertex_input_ctx(resource_description_t *p_description)
{
    return pipeline_add_vertex_input_ctx(p_description);
}

uint32_t vulkan_ops_mgr_get_pipeline_idx_setup_in_progress(void)
{
    for (uint32_t idx = 0; idx < MAX_NUM_VULKAN_PIPELINES; idx++) {
        if (pipeline_get_state(idx) != VULKAN_PIPELINE_STATE_SETUP) {
            continue;
        }

        return idx;
    }

    return FAILURE;
}

uint32_t vulkan_ops_mgr_init_pipeline_setup(void)
{
    uint32_t pipeline_entry_idx;

    if (vulkan_ops_mgr_get_pipeline_idx_setup_in_progress() != FAILURE) {
        printf("pipeline setup already in progress\n");

        return FAILURE;
    }

    if (pipeline_start_setup() == FAILURE) {
        return FAILURE;
    }

    pipeline_entry_idx = vulkan_ops_mgr_get_pipeline_idx_setup_in_progress();

    return pipeline_entry_idx;
}

uint32_t vulkan_ops_mgr_get_activated_pipeline_entry_idx(void)
{

}

uint32_t vulkan_ops_mgr_create_pipeline(VkDevice *p_device, uint32_t pipeline_entry_idx)
{
    if (pipeline_create(p_device, pipeline_entry_idx) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __vulkan_ops_mgr_get_activated_cmdbuf_idx(void)
{
    for (uint32_t idx = 0; idx < MAX_NUM_CMD_BUFFERS; idx++) {
        if (cmd_pool_get_cmd_buffer_state(idx) != VULKAN_CMD_POOL_CMDBUF_STATE_ACTIVATED) {
            continue;
        }
        return idx;
    }

    return MAX_NUM_CMD_BUFFERS;
}

uint32_t vulkan_ops_mgr_get_pipeline_submit_info(VkSubmitInfo *p_submit_info)
{
    uint32_t cmdbuf_idx;
    VkSubmitInfo submit_info;
    VkCommandBuffer *p_cmdbuf;

    cmdbuf_idx = __vulkan_ops_mgr_get_activated_cmdbuf_idx();
    if (cmdbuf_idx == MAX_NUM_CMD_BUFFERS) {
        printf("no command buffers bound to pipeline\n");
        return FAILURE;
    }

    p_cmdbuf = cmd_pool_get_cmd_buffer_object(cmdbuf_idx);

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = NULL;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = p_cmdbuf;

    submit_info.waitSemaphoreCount = 1;

    return SUCCESS;
}

VkPipeline *vulkan_ops_mgr_get_pipeline_object(uint32_t idx)
{
    if (pipeline_get_state(idx) == VULKAN_PIPELINE_STATE_CREATED) {
        return pipeline_get_pipeline_object(idx);
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

uint32_t vulkan_ops_mgr_check_cmd_buffer_activated(uint32_t buf_idx)
{
    if (cmd_pool_get_cmd_buffer_state(buf_idx) == VULKAN_CMD_POOL_CMDBUF_STATE_ACTIVATED) {
        return TRUE;
    }

    return FALSE;
}

uint32_t vulkan_ops_mgr_get_cmd_buffer_bitmap(uint32_t buf_idx)
{
    if (cmd_pool_get_cmd_buffer_state(buf_idx) == VULKAN_CMD_POOL_CMDBUF_STATE_ALLOCATED) {
        return 0;
    }

    return cmd_pool_get_cmd_buffer_bitmap(buf_idx);
}

uint32_t vulkan_ops_mgr_allocate_cmd_buffer(VkDevice *p_device, uint32_t family_idx,
                                                                    uint32_t buf_type)
{
    uint32_t res;
    uint32_t cmdbuf_idx;

    if (cmd_pool_get_state() != VULKAN_CMD_POOL_STATE_CREATED) {
        res = cmd_pool_create(p_device, family_idx);
        if (res == FAILURE) {
            return CMD_BUFFER_IDX_INVALID;
        }
    }

    return cmd_pool_allocate_buffer(p_device, buf_type);
}

uint32_t vulkan_ops_mgr_activate_cmd_buffer(uint32_t cmdbuf_idx)
{
    return cmd_pool_finish_buffer_recording(cmdbuf_idx);
}

VkCommandBuffer *vulkan_ops_mgr_get_activated_cmd_buffer_object(void)
{
    uint32_t cmdbuf_idx;

    cmdbuf_idx = __vulkan_ops_mgr_get_activated_cmdbuf_idx();
    if (cmdbuf_idx == MAX_NUM_CMD_BUFFERS) {
        printf("no command buffers allocated\n");
        return NULL;
    }

    return cmd_pool_get_cmd_buffer_object(cmdbuf_idx);
}

uint32_t vulkan_ops_mgr_free_cmd_buffer(VkDevice *p_device, uint32_t buf_idx)
{
    return cmd_pool_free_cmd_buffer(p_device, buf_idx);
}

uint32_t vulkan_ops_mgr_get_draw_cmd_subcmd_type(char *subcmd_name)
{
    const char *subcmd_names[NUM_VULKAN_SUBCMD_TYPES] = {
        "common",
        "index",
        "indirect"
    };

    if (!strcmp(subcmd_name, "common")) {
        return VULKAN_SUBCMD_TYPE_DRAW_COMMAND_COMMON;
    }
    else if (!strcmp(subcmd_name, "index")) {
        return VULKAN_SUBCMD_TYPE_DRAW_COMMAND_INDEX;
    }
    else if (!strcmp(subcmd_name, "indirect")) {
        return VULKAN_SUBCMD_TYPE_DRAW_COMMAND_INDIRECT;
    }

    return INVALID_VULKAN_SUBCMD_TYPE;
}

uint32_t vulkan_ops_mgr_add_vulkan_command(uint32_t cmd_type, vulkan_cmd_param_t *p_param)
{
    uint32_t res;

    if (!cmd_proc_ctx[cmd_type].add) {
        printf("command handler not implemented\n");
        return FAILURE;
    }

    cmd_proc_ctx[cmd_type].add(p_param);

    return SUCCESS;
}
