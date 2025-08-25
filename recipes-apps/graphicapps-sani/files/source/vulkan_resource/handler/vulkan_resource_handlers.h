#ifndef __VULKAN_RESOURCE_HANDLERS_H__
#define __VULKAN_RESOURCE_HANDLERS_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/resource_formats.h"

uint32_t resource_handler_setup_resource_buf(resource_t *p_resource_buf, resource_info_t *p_info,
                                                        resource_member_list_t *p_resource_members);

resource_description_t *resource_handler_get_resource_description(resource_info_t *p_info);

uint32_t resource_handler_get_resource_usage_flag(resource_info_t *p_info);

#endif

