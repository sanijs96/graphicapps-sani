#ifndef __RESOURCE_FORMATS_H__
#define __RESOURCE_FORMATS_H__

#include <vulkan/vulkan_core.h>

enum resource_format_types {
    RESOURCE_FORMAT_TYPE_VERTEX_2D_RGB,
    NUM_RESOURCE_FORMAT_TYPES,
};

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

typedef union resource_formats {
    vertex_2d_rgb_t vertex_2d_rgb;

} resource_t;







#endif
