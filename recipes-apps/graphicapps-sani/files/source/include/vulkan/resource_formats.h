#ifndef __RESOURCE_FORMATS_H__
#define __RESOURCE_FORMATS_H__

#include <vulkan/vulkan_core.h>
#include "common/common_def.h"

enum resource_format_types {
    RESOURCE_FORMAT_TYPE_NONE = 0,
    /* vertex buffer */
    RESOURCE_FORMAT_TYPE_VERTEX_BUFFER_START,
    RESOURCE_FORMAT_TYPE_VERTEX_BUFFER_2D_RGB = RESOURCE_FORMAT_TYPE_VERTEX_BUFFER_START,

    /* index buffer */
    RESOURCE_FORMAT_TYPE_INDEX_BUFFER_START,
    RESOURCE_FORMAT_TYPE_INDEX_BUFFER_3V = RESOURCE_FORMAT_TYPE_INDEX_BUFFER_START,

    /* image */
    RESOURCE_FORMAT_TYPE_IMAGE_START,

    RESOURCE_FORMAT_TYPE_INVALID,
};

#define MAX_NUM_RESOURCE_OBJECTS                (100)
#define MAX_NUM_RESOURCE_OBJECT_MEMBERS         (100)

#define MAX_RESOURCE_NAME_LENGTH                (MAX_LENGTH_ARGUMENT_NAME)
#define MAX_RESOURCE_VALUE_LENGTH_STR           (20)

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

    float padding;
} vertex_2d_rgb_t;

typedef union index_3v {
    uint32_t idx[3];
} index_3v_t;

typedef union resource {
    vertex_2d_rgb_t vertex_2d_rgb;
    index_3v_t index_3v;
} resource_t;

typedef struct resource_info {
    uint32_t type;
    uint32_t count;

    // resource specific information
    union {
        struct {
            uint32_t index_type;
        } index_buffer;
        struct {
            uint32_t binding_idx;
        } vertex_buffer;
    };

    uint32_t usage_flag;

    char name[MAX_LENGTH_ARGUMENT_NAME];
} resource_info_t;

typedef union {
    struct {
        uint32_t attribute_count;
        VkVertexInputBindingDescription binding;
        VkVertexInputAttributeDescription *p_attributes;
    } vertex_buffer;
    struct {
        uint32_t index_type;
    } index_buffer;

} resource_description_t;

typedef struct {
    char value[MAX_RESOURCE_VALUE_LENGTH_STR];
} resource_member_entry_t, *resource_member_list_t;

#endif
