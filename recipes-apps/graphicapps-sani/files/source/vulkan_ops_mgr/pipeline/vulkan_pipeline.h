#ifndef __VULKAN_PIPELINE_H__
#define __VULKAN_PIPELINE_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/pipeline_stages.h"

enum pipeline_creation_state {
    VULKAN_PIPELINE_STATE_DEFAULT = 0,
    VULKAN_PIPELINE_STATE_VIEWPORT_REGISTERED,
    VULKAN_PIPELINE_STATE_READY,
    VULKAN_PIPELINE_STATE_CREATED,
    VULKAN_PIPELINE_STATE_INVALID,
};

uint32_t pipeline_get_creation_state(uint32_t stage);

uint32_t pipeline_add_shader_file(uint32_t stage, char *filename, VkDevice *p_device);

uint32_t pipeline_add_viewport_ctx(VkExtent2D *p_extent, VkFormat *p_format);

uint32_t pipeline_create(VkDevice *p_device);

uint32_t pipeline_show_pipeline_info(uint32_t stage);
#endif
