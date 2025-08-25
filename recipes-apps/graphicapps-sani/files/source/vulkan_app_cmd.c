#include <stdio.h>
#include <string.h>

#include "common/common_def.h"
#include "vulkan/common_def.h"

#include "vulkan/cmd_types.h"
#include "vulkan/queue_types.h"
#include "vulkan/function_scope.h"
#include "vulkan/pipeline_stages.h"
#include "vulkan/resource_formats.h"

#include "vulkan_app_cmd.h"

#include "app_utilities/datascript_parser.h"

#include "vulkan_object/vulkan_obj_mgr.h"
#include "vulkan_operation/vulkan_ops_mgr.h"
#include "vulkan_resource/vulkan_resource_mgr.h"

#include "window_object/window_obj_mgr.h"

typedef union vulkan_cmd_args_list {
    struct {
        uint32_t scope;
        uint32_t phydev_idx;
        char name[VK_MAX_EXTENSION_NAME_SIZE];
    } layer;

    struct {
        uint32_t scope;
        uint32_t phydev_idx;
        char name[VK_MAX_EXTENSION_NAME_SIZE];
    } extension;

    struct {
        uint32_t phydev_idx;
        uint32_t show_extension;
    } device;

    struct {
        uint32_t stage_idx;
        char resource_name[MAX_LENGTH_ARGUMENT_NAME];
        char filename[FILENAME_MAX];
    } pipeline;

    struct {
        uint32_t buf_idx;
        uint32_t buf_type;
        uint32_t cmd_type;
        uint32_t pipeline_idx;
        char resource_name[MAX_LENGTH_ARGUMENT_NAME];
    } cmdbuf;

    struct {
        uint32_t pipeline_idx;
        char name[FILENAME_MAX];
    } resource;

} vulkan_cmd_args_list_t;

static uint32_t __setup_layer_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(LAYER_NAME):
            strcpy(p_arglist->layer.name, arg.value);
            break;

        case PARAM_VK(LAYER_SCOPE):
            p_arglist->layer.scope = (uint32_t)(*(char *)arg.value - '0');
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_extension_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(EXTENSION_NAME):
            strcpy(p_arglist->extension.name, arg.value);
            break;

        case PARAM_VK(EXTENSION_SCOPE):
            p_arglist->extension.scope = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(EXTENSION_PHYDEV_IDX):
            p_arglist->extension.phydev_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_device_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(DEVICE_PHYDEV_IDX):
            p_arglist->device.phydev_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(DEVICE_SHOW_EXTENSION):
            p_arglist->device.show_extension = ((uint32_t)(*(char *)arg.value - '0') == 1) ?
                                                                                TRUE : FALSE;
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_instance_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    return SUCCESS;
}

static uint32_t __setup_pipeline_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(PIPELINE_STAGE_TYPE):
            p_arglist->pipeline.stage_idx =
                vulkan_ops_mgr_get_pipeline_stage_idx_from_name(arg.value);
            break;

        case PARAM_VK(PIPELINE_SHADER_FILENAME):
            strcpy(p_arglist->pipeline.filename, arg.value);
            break;

        case PARAM_VK(PIPELINE_RESOURCE_NAME):
            strcpy(p_arglist->pipeline.resource_name, arg.value);
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_cmdbuf_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(CMDBUF_BUFFER_TYPE):
            p_arglist->cmdbuf.buf_type = vulkan_ops_mgr_get_vulkan_cmd_buffer_type(arg.value);
            break;

        case PARAM_VK(CMDBUF_COMMAND_TYPE):
            p_arglist->cmdbuf.cmd_type = vulkan_ops_mgr_get_vulkan_cmd_type_from_name(arg.value);
            break;

        case PARAM_VK(CMDBUF_PIPELINE_IDX):
            p_arglist->cmdbuf.pipeline_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(CMDBUF_BUFFER_IDX):
            p_arglist->cmdbuf.buf_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        case PARAM_VK(CMDBUF_RESOURCE_NAME):
            strcpy(p_arglist->cmdbuf.resource_name, arg.value);
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __setup_resource_argument(command_arg_t arg, vulkan_cmd_args_list_t *p_arglist)
{
    switch (arg.type) {
        case PARAM_VK(RESOURCE_OBJECT_NAME):
            strcpy(p_arglist->resource.name, arg.value);
            break;

        case PARAM_VK(RESOURCE_PIPELINE_IDX):
            p_arglist->cmdbuf.pipeline_idx = (uint32_t)(*(char *)arg.value - '0');
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}

static uint32_t vulkan_app_cmd_check_arg_exist(command_t *p_cmd, uint32_t arg_type)
{
    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        if (p_cmd->p_args[idx].type == arg_type) {
            return TRUE;
        }
    }

    return FALSE;
}

static uint32_t vulkan_app_cmd_setup_argument_list(command_t *p_cmd,
                                        vulkan_cmd_args_list_t *p_arglist)
{
    uint32_t res;
    uint32_t (*__arg_setup_func)(command_arg_t, vulkan_cmd_args_list_t *);

    if (p_cmd->num_args == 0) {
        return SUCCESS;
    }

    if (!strcmp(p_cmd->cmd_name, "layer")) {
        __arg_setup_func = __setup_layer_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "extension")) {
        __arg_setup_func = __setup_extension_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "device")) {
        __arg_setup_func = __setup_device_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "instance")) {
        __arg_setup_func = __setup_instance_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "pipeline")) {
        __arg_setup_func = __setup_pipeline_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "cmdbuf")) {
        __arg_setup_func = __setup_cmdbuf_argument;
    }
    else if (!strcmp(p_cmd->cmd_name, "resource")) {
        __arg_setup_func = __setup_resource_argument;
    }
    else {
        return FAILURE;
    }

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        res = __arg_setup_func(p_cmd->p_args[idx], p_arglist);
        if (res == FAILURE) {
            break;
        }
    }

    return res;
}

uint32_t vulkan_app_cmd_enable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_enable_layer(args_list.layer.name);
}

uint32_t vulkan_app_cmd_disable_layer(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_disable_layer(args_list.layer.name);
}

uint32_t vulkan_app_cmd_show_layers_list(command_t *p_cmd)
{
    vulkan_obj_mgr_show_layers_list();

    return  SUCCESS;
}

uint32_t vulkan_app_cmd_enable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        return vulkan_obj_mgr_enable_extension(args_list.extension.name);
    }
    else if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        if (vulkan_app_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FALSE) {
            printf("no physical device selected\n");

            return FAILURE;
        }

        return vulkan_obj_mgr_enable_phydev_extension(args_list.extension.name,
                                                        args_list.extension.phydev_idx);
    }
    else {
        return FAILURE;
    }
}

uint32_t vulkan_app_cmd_disable_extension(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        return vulkan_obj_mgr_enable_extension(args_list.extension.name);
    }
    else if (args_list.extension.scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        if (vulkan_app_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FALSE) {
            printf("no physical device selected\n");

            return FAILURE;
        }

        return vulkan_obj_mgr_disable_phydev_extension(args_list.extension.name,
                                                        args_list.extension.phydev_idx);
    }
    else {
        return FAILURE;
    }
}

uint32_t vulkan_app_cmd_show_extensions_list(command_t *p_cmd)
{
    vulkan_obj_mgr_show_extensions_list();

    return SUCCESS;
}

uint32_t vulkan_app_cmd_create_instance(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;
    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    return vulkan_obj_mgr_create_instance();
}

uint32_t vulkan_app_cmd_delete_instance(command_t *p_cmd)
{
    return vulkan_obj_mgr_delete_instance();
}

uint32_t vulkan_app_cmd_create_device(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_app_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FALSE) {
        return FAILURE;
    }

    if (vulkan_obj_mgr_create_device(args_list.device.phydev_idx) == FAILURE) {
        return FAILURE;
    }

    VkDevice *p_device;
    VkPhysicalDevice *p_phydev;

    p_device = vulkan_obj_mgr_get_current_device_object();
    p_phydev = vulkan_obj_mgr_get_phydev_object(args_list.device.phydev_idx);

    vulkan_resource_mgr_add_memory_property(p_phydev);

    window_obj_mgr_setup_device_ctx(p_phydev, p_device);
}

static void __check_presentation_support(uint32_t phydev_idx, VkSurfaceKHR *p_surface)
{
    uint32_t queue_count;
    uint32_t present_support;
    VkPhysicalDevice *p_phydev;

    p_phydev = vulkan_obj_mgr_get_phydev_object(phydev_idx);

    queue_count = vulkan_obj_mgr_get_device_queue_family_count(phydev_idx);

    printf("Presentation Queue Index: ");
    for (uint32_t queue_idx = 0; queue_idx < queue_count; queue_idx++)  {
        present_support = FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(*p_phydev, queue_idx, *p_surface, &present_support);

        if (present_support == TRUE) {
            printf("%u ", queue_idx);
        }
    }
    printf("\n");

    return;
}

static void __vulkan_app_cmd_show_display_support(uint32_t phydev_idx)
{
    VkSurfaceKHR *p_display_object;

    p_display_object = window_obj_mgr_get_current_display_object();

    __check_presentation_support(phydev_idx, p_display_object);
}

uint32_t vulkan_app_cmd_show_devices_list(command_t *p_cmd)
{
    uint32_t phydev_idx;
    uint32_t phydev_count;
    vulkan_cmd_args_list_t args_list;

    if (vulkan_obj_mgr_get_instance_object() == NULL) {
        printf("instance not created yet\n");
        return FAILURE;
    }

    phydev_count = vulkan_obj_mgr_get_phydev_count();

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    printf("[DEVICE LIST]\n");
    if (vulkan_app_cmd_check_arg_exist(p_cmd, PARAM_VK(EXTENSION_PHYDEV_IDX)) == FALSE) {
        phydev_idx = 0;
    }
    else {
        phydev_idx = args_list.device.phydev_idx;

        if (phydev_idx >= phydev_count) {
            printf("invalid device index\n");
            return FAILURE;
        }

        phydev_count = args_list.device.phydev_idx + 1;
    }

    for (; phydev_idx < phydev_count; phydev_idx++) {
        vulkan_obj_mgr_show_device_info(phydev_idx);

        if (args_list.device.show_extension == TRUE) {
            vulkan_obj_mgr_show_device_extensions_list(phydev_idx);
        }

        if (phydev_idx == vulkan_obj_mgr_get_current_phydev_idx()) {
            if (vulkan_obj_mgr_check_device_created() == TRUE) {
                vulkan_resource_mgr_show_device_memory_properties();
            }
        }

        if (window_obj_mgr_check_display_status() == WINDOW_OBJ_DISPLAY_STATE_CREATED) {
            __vulkan_app_cmd_show_display_support(phydev_idx);
        }
    }

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_add_shader_ctx(uint32_t stage_idx, char *filename)
{
    VkDevice *p_device;

    p_device = vulkan_obj_mgr_get_current_device_object();
    if (p_device == NULL) {
        return FAILURE;
    }

    if (vulkan_ops_mgr_add_shader_file(stage_idx, filename, p_device) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_add_viewport_ctx(void)
{
    VkFormat *p_format;
    VkExtent2D *p_extent;

    if (window_obj_mgr_check_display_status() != WINDOW_OBJ_DISPLAY_STATE_CREATED) {
        printf("display is not started yet\n");
        return FAILURE;
    }

    p_extent = window_obj_mgr_get_current_swapchain_extent();
    p_format = window_obj_mgr_get_current_swapchain_format();

    if (vulkan_ops_mgr_add_viewport_ctx(p_extent, p_format) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_add_vertex_input_ctx(char *resource_name)
{
    resource_description_t *p_description;

    p_description = vulkan_resource_mgr_get_resource_description(resource_name);
    if (p_description == NULL) {
        return FAILURE;
    }

    if (vulkan_ops_mgr_add_vertex_input_ctx(p_description) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t vulkan_app_cmd_add_pipeline_stage(command_t *p_cmd)
{
    uint32_t res;
    uint32_t stage_idx;

    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    stage_idx = args_list.pipeline.stage_idx;

    switch (stage_idx) {
        case VULKAN_PIPELINE_STAGE_VERTEX_INPUT:
            res = __vulkan_app_cmd_add_vertex_input_ctx(args_list.pipeline.resource_name);
            break;

        case VULKAN_PIPELINE_STAGE_VERTEX_SHADER:
        case VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER:
            res = __vulkan_app_cmd_add_shader_ctx(stage_idx, args_list.pipeline.filename);
            break;

        case VULKAN_PIPELINE_STAGE_VIEWPORT:
            res = __vulkan_app_cmd_add_viewport_ctx();
            break;

        default:
            printf("changing stage of %u is not allowed (yet)\n", stage_idx);
            return FAILURE;
    }

    return res;
}

uint32_t vulkan_app_cmd_create_pipeline(command_t *p_cmd)
{
    uint32_t pipeline_entry_idx;
    VkDevice *p_device;
    VkRenderPass *p_renderpass;
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    p_device = vulkan_obj_mgr_get_current_device_object();
    if (p_device == NULL) {
        printf("device is not created\n");

        return FAILURE;
    }

    if (window_obj_mgr_check_display_status() != WINDOW_OBJ_DISPLAY_STATE_CREATED) {
        printf("window object is not created\n");

        return FAILURE;
    }

    if (vulkan_ops_mgr_get_pipeline_idx_setup_in_progress() == FAILURE) {
        return FAILURE;
    }

    pipeline_entry_idx = vulkan_ops_mgr_get_pipeline_idx_setup_in_progress();

    if (vulkan_ops_mgr_create_pipeline(p_device, pipeline_entry_idx) == SUCCESS) {
        p_renderpass = vulkan_ops_mgr_get_renderpass_object();

        window_obj_mgr_create_framebuffers(p_renderpass);
    }

    p_cmd->retval = pipeline_entry_idx;

    return SUCCESS;
}

uint32_t vulkan_app_cmd_init_pipeline_ctx(command_t *p_cmd)
{
    uint32_t res;
    uint32_t pipeline_entry_idx;
    VkPipeline *p_pipeline;
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_ops_mgr_init_pipeline_setup() == FAILURE) {
        return FAILURE;
    }

    pipeline_entry_idx = vulkan_ops_mgr_get_pipeline_idx_setup_in_progress();

    p_cmd->retval = pipeline_entry_idx;

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_add_signal_semaphores(VkDevice *p_device)
{
    uint32_t res;
    VkSemaphore signal_semaphore;
    VkSemaphoreCreateInfo semaphore_info;

    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = 0;
    semaphore_info.pNext = NULL;

    res = vkCreateSemaphore(*p_device, &semaphore_info, NULL, &signal_semaphore);
    if (res != VK_SUCCESS) {
        printf("signal semaphore create failure: %d\n", res);
        return FAILURE;
    }

    if (vulkan_obj_mgr_add_signal_semaphore(&signal_semaphore) == FAILURE) {
        printf("signal semaphore add failure\n");
        return FAILURE;
    }

    window_obj_mgr_add_swapchain_signal_semaphore(&signal_semaphore);

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_add_wait_semaphores(VkDevice *p_device)
{
    VkSemaphore *p_window_semaphore;
    VkPipelineStageFlags semaphore_stage;

    p_window_semaphore = window_obj_mgr_get_display_semaphore_object();

    semaphore_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    if (vulkan_obj_mgr_add_wait_semaphore(p_window_semaphore, semaphore_stage) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_queue_submit_info(VkSubmitInfo *p_submit_info)
{
    VkCommandBuffer *p_cmdbuf;

    p_submit_info->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    p_submit_info->pNext = NULL;

    p_cmdbuf = vulkan_ops_mgr_get_activated_cmd_buffer_object();
    if (p_cmdbuf == NULL) {
        printf("no command buffers bound to pipeline\n");
        return FAILURE;
    }

    p_submit_info->commandBufferCount = 1;
    p_submit_info->pCommandBuffers = p_cmdbuf;

    p_submit_info->pWaitDstStageMask = vulkan_obj_mgr_get_wait_semaphore_stages();

    p_submit_info->pWaitSemaphores = vulkan_obj_mgr_get_wait_semaphore_objects();
    p_submit_info->waitSemaphoreCount = vulkan_obj_mgr_get_wait_semaphore_count();

    p_submit_info->pSignalSemaphores = vulkan_obj_mgr_get_signal_semaphore_objects();
    p_submit_info->signalSemaphoreCount = vulkan_obj_mgr_get_signal_semaphore_count();

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_submit_queue(VkQueue *p_queue, uint32_t type)
{
    VkSubmitInfo submit_info;

    if (__vulkan_app_cmd_setup_queue_submit_info(&submit_info) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_obj_mgr_submit_queue(p_queue, &submit_info) == FAILURE) {
        return FAILURE;
    }
}

static uint32_t __vulkan_app_cmd_run_cmd_buffer(vulkan_cmd_args_list_t *p_args_list)
{
    uint32_t queue_idx;
    uint32_t queue_type;
    uint32_t cmd_buffer_bitmap;
    VkQueue *p_queue;
    VkDevice *p_device;

    queue_type = VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS;
    queue_idx = vulkan_obj_mgr_select_available_queue_idx(queue_type);
    cmd_buffer_bitmap = vulkan_ops_mgr_get_cmd_buffer_bitmap(p_args_list->cmdbuf.buf_idx);

    p_queue = vulkan_obj_mgr_get_queue_object(queue_type, queue_idx);
    if (p_queue == NULL) {
        return FAILURE;
    }

    p_device = vulkan_obj_mgr_get_current_device_object();
    if (p_device == NULL) {
        return FAILURE;
    }

    if (window_obj_mgr_check_display_status() != WINDOW_OBJ_DISPLAY_STATE_CREATED) {
        return FAILURE;
    }

    if (__vulkan_app_cmd_submit_queue(p_queue, queue_type) == FAILURE) {
        return FAILURE;
    }

    if (!(cmd_buffer_bitmap & (1 << VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS))) {
        goto exit;
    }

    if (window_obj_mgr_show_queue_result(p_queue) == FAILURE) {
        return FAILURE;
    }

exit:
    if (vulkan_ops_mgr_free_cmd_buffer(p_device, p_args_list->cmdbuf.buf_idx) == FAILURE) {
        return FAILURE;
    }

    vulkan_obj_mgr_release_queue(queue_type, queue_idx);

    return SUCCESS;
}

uint32_t vulkan_app_cmd_show_pipeline_info(command_t *p_cmd)
{

}

uint32_t vulkan_app_cmd_allocate_command_buffer(command_t *p_cmd)
{
    uint32_t cmd_buf_idx;
    uint32_t cmd_buf_type;
    uint32_t queue_family_idx;
    VkDevice *p_device;
    VkRenderPass *p_renderpass;
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    p_device = vulkan_obj_mgr_get_current_device_object();
    if (p_device == NULL) {
        printf("device is not created\n");
        return FAILURE;
    }

    cmd_buf_type = args_list.cmdbuf.buf_type;
    if (cmd_buf_type == NUM_VULKAN_CMD_POOL_CMDBUF_TYPES) {
        return FAILURE;
    }

    queue_family_idx = vulkan_obj_mgr_get_graphics_queue_family_idx(p_device);

    cmd_buf_idx = vulkan_ops_mgr_allocate_cmd_buffer(p_device, queue_family_idx, cmd_buf_type);
    if (cmd_buf_idx == CMD_BUFFER_IDX_INVALID) {
        return FAILURE;
    }

    p_cmd->retval = cmd_buf_idx;

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_renderpass_command_param(vulkan_cmd_param_t *p_param)
{
    VkDevice *p_device;
    const VkClearValue clear_color = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};

    p_device = vulkan_obj_mgr_get_current_device_object();

    if (p_device == NULL) {
        printf("device not created yet\n");
        return FAILURE;
    }

    if (window_obj_mgr_start_display(p_device) == FAILURE) {
        return FAILURE;
    }

    if (__vulkan_app_cmd_add_wait_semaphores(p_device) == FAILURE) {
        return FAILURE;
    }

    if (__vulkan_app_cmd_add_signal_semaphores(p_device) == FAILURE) {
        return FAILURE;
    }

    p_param->renderpass.clear_value_count = 1;
    p_param->renderpass.p_clear_values = (VkClearValue *)&clear_color;
    p_param->renderpass.p_renderpass = vulkan_ops_mgr_get_renderpass_object();

    p_param->renderpass.p_framebuffers = window_obj_mgr_get_framebuffer_objects();
    p_param->renderpass.framebuffer_count = window_obj_mgr_get_framebuffer_object_count();
    p_param->renderpass.framebuffer_image_idx = window_obj_mgr_get_next_framebuffer_image_idx();

    p_param->renderpass.p_swapchain_extent = window_obj_mgr_get_current_swapchain_extent();

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_bind_pipeline_command_param(vulkan_cmd_param_t *p_param,
                                                                            uint32_t pipeline_idx)
{
    // TODO: check pipeline created

    p_param->bind_pipeline.p_pipeline = vulkan_ops_mgr_get_pipeline_object(pipeline_idx);

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_bind_resource_command_param(vulkan_cmd_param_t *p_param,
                                                                            char *resource_name)
{
    p_param->bind_resource.p_resource_info = vulkan_resource_mgr_get_resource_info(resource_name);
    if (p_param->bind_resource.p_resource_info == NULL) {
        return FAILURE;
    }

    p_param->bind_resource.p_resource = vulkan_resource_mgr_get_resource_object(resource_name);
    if (p_param->bind_resource.p_resource == NULL) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_draw_command_param(vulkan_cmd_param_t *p_param)
{
    VkRect2D scissor;
    VkViewport viewport;
    VkExtent2D *p_swapchain_extent;

    if (window_obj_mgr_check_display_status() != WINDOW_OBJ_DISPLAY_STATE_CREATED) {
        printf("display not started yet\n");
        return FAILURE;
    }

    p_swapchain_extent = window_obj_mgr_get_current_swapchain_extent();

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.height = (float)p_swapchain_extent->height;
    viewport.width = (float)p_swapchain_extent->width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.extent = *p_swapchain_extent;
    scissor.offset = (VkOffset2D){0, 0};

    p_param->draw.scissor = scissor;
    p_param->draw.viewport = viewport;

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_copy_resource_command_param(vulkan_cmd_param_t *p_param,
                                                                            char *resource_name)
{
    uint32_t datasize;
    VkDevice *p_device;
    resource_info_t *p_info;

    p_device = vulkan_obj_mgr_get_current_device_object();
    if (p_device == NULL) {
        printf("device not created\n");
        return FAILURE;
    }

    p_info = vulkan_resource_mgr_get_resource_info(resource_name);
    if (p_info == NULL) {
        return FAILURE;
    }

    datasize = vulkan_resource_mgr_get_resource_data_unit_size(p_info->type) * p_info->count;
    p_param->copy_resource.datasize = datasize;

    p_param->copy_resource.p_object_src = vulkan_resource_mgr_get_resource_object(resource_name);
    if (p_param->copy_resource.p_object_src == NULL) {
        return FAILURE;
    }

    if (vulkan_resource_mgr_create_device_resource_copy(p_device,
                resource_name, p_param->copy_resource.p_object_dst) == FAILURE) {
        return FAILURE;
    };

    return SUCCESS;
}

static uint32_t __vulkan_app_cmd_setup_command_param(uint32_t cmd_type,
                                                        vulkan_cmd_param_t *p_param,
                                                        vulkan_cmd_args_list_t *p_args)
{
    uint32_t res;

    switch (cmd_type) {
        case VULKAN_SUPPORTED_CMD_TYPE_RENDERPASS:
            res = __vulkan_app_cmd_setup_renderpass_command_param(p_param);
            break;

        case VULKAN_SUPPORTED_CMD_TYPE_BIND_PIPELINE:
            res = __vulkan_app_cmd_setup_bind_pipeline_command_param(p_param,
                                                                     p_args->cmdbuf.pipeline_idx);
            break;

        case VULKAN_SUPPORTED_CMD_TYPE_BIND_RESOURCE:
            res = __vulkan_app_cmd_setup_bind_resource_command_param(p_param,
                                                                     p_args->cmdbuf.resource_name);
            break;

        case VULKAN_SUPPORTED_CMD_TYPE_DRAW:
            res = __vulkan_app_cmd_setup_draw_command_param(p_param);
            break;

        case VULKAN_SUPPORTED_CMD_TYPE_COPY_RESOURCE:
            res = __vulkan_app_cmd_setup_copy_resource_command_param(p_param,
                                                                     p_args->cmdbuf.resource_name);
            break;

        default:
            return FAILURE;
    }

    return res;
}

uint32_t vulkan_app_cmd_add_vulkan_command(command_t *p_cmd)
{
    uint32_t res;
    uint32_t cmd_type;
    uint32_t cmdbuf_idx;
    vulkan_cmd_param_t cmd_param;
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    cmd_type = args_list.cmdbuf.cmd_type;

    cmdbuf_idx = args_list.cmdbuf.buf_idx;

    if ((vulkan_app_cmd_check_arg_exist(p_cmd, PARAM_VK(CMDBUF_BUFFER_IDX)) == FALSE) ||
        (vulkan_app_cmd_check_arg_exist(p_cmd, PARAM_VK(CMDBUF_COMMAND_TYPE)) == FALSE)) {
        printf("argument not specified\n");
        return FAILURE;
    }

    if (vulkan_ops_mgr_check_cmd_buffer_allocated(cmdbuf_idx) == FALSE) {
        printf("buffer not allocated\n");
        return FAILURE;
    }

    if (__vulkan_app_cmd_setup_command_param(cmd_type, &cmd_param, &args_list) == FAILURE) {
        printf("command param setup failure\n");
        return FAILURE;
    }

    cmd_param.cmdbuf_idx = cmdbuf_idx;

    res = vulkan_ops_mgr_add_vulkan_command(cmd_type, &cmd_param);

    return res;
}

uint32_t vulkan_app_cmd_show_command_buffer_info(command_t *p_cmd)
{

}

uint32_t vulkan_app_cmd_run_commands(command_t *p_cmd)
{
    vulkan_cmd_args_list_t args_list;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_ops_mgr_activate_cmd_buffer(args_list.cmdbuf.buf_idx) == FAILURE) {
        printf("command buffer not activated\n");
        return FAILURE;
    }

    __vulkan_app_cmd_run_cmd_buffer(&args_list);

    return SUCCESS;
}

uint32_t vulkan_app_cmd_create_resource(command_t *p_cmd)
{
    uint32_t res;
    VkDevice *p_device;
    resource_info_t *p_info;
    vulkan_cmd_args_list_t args_list;
    resource_member_list_t *p_resource_buf;

    if (vulkan_app_cmd_setup_argument_list(p_cmd, &args_list) == FAILURE) {
        return FAILURE;
    }

    if (vulkan_obj_mgr_check_device_created() == FALSE) {
        printf("create device first\n");
        return FAILURE;
    }

    p_device = vulkan_obj_mgr_get_current_device_object();

    p_info = vulkan_resource_mgr_get_resource_info(args_list.resource.name);
    if (p_info == NULL) {
        return FAILURE;
    }

    p_resource_buf = datascript_get_resource_data(args_list.resource.name);
    if (p_resource_buf == NULL) {
        return FAILURE;
    }

    if (p_info->type < MAX_RESOURCE_FORMAT_TYPE_BUFFERS) {
        res = vulkan_resource_mgr_create_vertex_buffer(p_device, args_list.resource.name,
                                                                             p_resource_buf);
    }
    else if (p_info->type < MAX_RESOURCE_FORMAT_TYPE_BUFFERS) { // TODO
        res = vulkan_resource_mgr_create_buffer(p_device, args_list.resource.name,
                                                                    p_resource_buf);
    }
    else {
        res = vulkan_resource_mgr_create_image(p_device, args_list.resource.name,
                                                                    p_resource_buf);
    }

    if (res == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}
