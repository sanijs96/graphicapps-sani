#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>

#include "common/common_def.h"

#include "vulkan_resource_handlers.h"

typedef resource_description_t* (*resource_bind_fn)(resource_info_t *p_info);
typedef uint32_t (*resource_data_setup_fn)(resource_t *p_data_buf, resource_info_t *p_info,
                                                            resource_member_list_t *p_members);

typedef const struct resource_handler {
    char *name;
    resource_bind_fn bind_fn;
    resource_data_setup_fn setup_fn;
} resource_handler_t;

static uint32_t __resource_handler_setup_vertex_2d_rgb(resource_t *p_buf, resource_info_t *p_info,
                                                                resource_member_list_t *p_members);
static resource_description_t *__resource_handler_bind_vertex_2d_rgb(resource_info_t *p_info);

const resource_handler_t resource_handlers[] = {
    [RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_START] = {.name = NULL},
    [RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_2D_RGB] = {
        .setup_fn = __resource_handler_setup_vertex_2d_rgb,
        .bind_fn = __resource_handler_bind_vertex_2d_rgb,
    },
    [RESOURCE_FORMAT_TYPE_BUFFER_START] = {.name = NULL},


    [RESOURCE_FORMAT_TYPE_IMAGE_START] = {.name = NULL},
};

static uint32_t __resource_handler_setup_vertex_2d_rgb(resource_t *p_buf, resource_info_t *p_info,
                                                                resource_member_list_t *p_members)
{
    uint32_t member_idx;
    char *p_value_str;

    for (uint32_t idx = 0; idx < p_info->count; idx++) {
        member_idx = 0;

        p_buf[idx].vertex_2d_rgb.pos.x = atof(p_members[idx][member_idx++].value);
        p_buf[idx].vertex_2d_rgb.pos.y = atof(p_members[idx][member_idx++].value);

        p_buf[idx].vertex_2d_rgb.color.r = atof(p_members[idx][member_idx++].value);
        p_buf[idx].vertex_2d_rgb.color.g = atof(p_members[idx][member_idx++].value);
        p_buf[idx].vertex_2d_rgb.color.b = atof(p_members[idx][member_idx++].value);
    }

    return SUCCESS;
}

static resource_description_t *__resource_handler_bind_vertex_2d_rgb(resource_info_t *p_info)
{
    resource_description_t *p_description;
    VkVertexInputAttributeDescription *p_attributes;

    p_description = (resource_description_t *)malloc(sizeof(resource_description_t));

    p_description->vertex_buffer.attribute_count = 2;

    p_attributes = (VkVertexInputAttributeDescription *)
                        malloc(sizeof(VkVertexInputAttributeDescription) *
                                p_description->vertex_buffer.attribute_count);

    p_description->vertex_buffer.p_attributes = p_attributes;

    p_attributes[0].location = 0;
    p_attributes[0].binding = p_info->binding;
    p_attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
    p_attributes[0].offset = offsetof(vertex_2d_rgb_t, pos);

    p_attributes[1].location = 1;
    p_attributes[1].binding = p_info->binding;
    p_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    p_attributes[1].offset = offsetof(vertex_2d_rgb_t, color);

    p_description->vertex_buffer.binding.binding = p_info->binding;
    p_description->vertex_buffer.binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    p_description->vertex_buffer.binding.stride = sizeof(vertex_2d_rgb_t);

    return p_description;
}

static resource_handler_t *__resource_handler_get_resource_handler(uint32_t type)
{
    uint32_t list_size;

    list_size = sizeof(resource_handlers) / sizeof(resource_handler_t);
    if (type > list_size) {
        printf("resource handler not detected\n");
        return NULL;
    }

    return &resource_handlers[type];
}

uint32_t resource_handler_setup_resource_buf(resource_t *p_resource_buf, resource_info_t *p_info,
                                                        resource_member_list_t *p_resource_members)
{
    resource_handler_t *p_handler;

    p_handler = __resource_handler_get_resource_handler(p_info->type);
    if (p_handler == NULL) {
        return FAILURE;
    }

    if (p_handler->setup_fn(p_resource_buf, p_info, p_resource_members) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

resource_description_t *resource_handler_get_resource_description(resource_info_t *p_info)
{
    resource_handler_t *p_handler;

    p_handler = __resource_handler_get_resource_handler(p_info->type);
    if (p_handler == NULL) {
        return NULL;
    }

    return p_handler->bind_fn(p_info);
}
