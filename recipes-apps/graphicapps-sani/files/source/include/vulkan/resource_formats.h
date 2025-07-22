#ifndef __RESOURCE_FORMATS_H__
#define __RESOURCE_FORMATS_H__

#include <vulkan/vulkan_core.h>
#include "common/common_def.h"

enum resource_format_types {
    RESOURCE_FORMAT_TYPE_VERTEX_2D_RGB,
    NUM_RESOURCE_FORMAT_TYPES,
};

#define MAX_NUM_RESOURCE_MEMBERS    (100)

typedef struct vertex_2d_rgb {
    struct {
        float x;
        float y;
    } pos;

    struct {
        float r;
        float g;
        float b;
    } color;
} vertex_2d_rgb_t;

typedef union resource {
    vertex_2d_rgb_t vertex_2d_rgb;
} resource_t;

typedef struct resource_info {
    uint32_t type;
    uint32_t count;

    char name[MAX_LENGTH_ARGUMENT_NAME];
} resource_info_t;

#endif
