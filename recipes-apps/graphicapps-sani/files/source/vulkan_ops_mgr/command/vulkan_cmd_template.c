#include <stdio.h>

#include "common/common_def.h"
#include "vulkan/cmd_types.h"

#include "vulkan_cmd_template.h"

uint32_t cmd_template_setup_renderpass_command(vulkan_cmd_template_t *p_template,
                                                        vulkan_cmd_param_t *p_param)
{
    VkRenderPassBeginInfo *p_renderpass_info;

    p_renderpass_info = &p_template->renderpass;

    p_renderpass_info->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    p_renderpass_info->pNext = NULL;
    p_renderpass_info->renderPass = VK_NULL_HANDLE;
    p_renderpass_info->framebuffer = VK_NULL_HANDLE;
    p_renderpass_info->renderArea = (VkRect2D){0, 0};
    p_renderpass_info->clearValueCount = 0;
    p_renderpass_info->pClearValues = NULL;
    p_renderpass_info->renderPass = *p_param->renderpass.p_renderpass;
    p_renderpass_info->renderArea.offset = (VkOffset2D){0, 0};
    p_renderpass_info->renderArea.extent = *p_param->renderpass.p_swapchain_extent;

    p_renderpass_info->clearValueCount = p_param->renderpass.clear_value_count;
    p_renderpass_info->pClearValues = p_param->renderpass.p_clear_values;

    for (uint32_t idx = 0; idx < p_param->renderpass.framebuffer_count; idx++) {
        p_renderpass_info->framebuffer = p_param->renderpass.p_framebuffers[idx];
    }

    return SUCCESS;
}
