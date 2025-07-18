#ifndef __VULKAN_RESOURCE_MGR_H__
#define __VULKAN_RESOURCE_MGR_H__

#include <vulkan/vulkan_core.h>

#include "vulkan/resource_formats.h"

typedef struct resource_data {
    uint32_t type;
    uint32_t count;
    char *data_name;
    resource_t *p_data;
} resource_data_t;

void vulkan_resource_mgr_init(void);

uint32_t vulkan_resource_mgr_create_vertex_buffer(resource_data_t *p_data);

#endif
