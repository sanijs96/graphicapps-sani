#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "vulkan_resource_mgr.h"

typedef struct resource_entry {
    char *name;
    resource_t *p_data;

    struct resource_entry *p_next;
    struct resource_entry *p_prev;
} resource_entry_t;

typedef resource_entry_t resource_list_t;

static struct {
    resource_list_t image_list;
    resource_list_t buffer_list;
    resource_list_t vertex_list;
} resource_ctx;

static resource_entry_t *__vulkan_resource_mgr_create_new_entry(resource_list_t *p_list)
{
    resource_entry_t *p_entry;

    p_entry = p_list;

    while (p_entry->p_next != NULL) {
        p_entry = p_entry->p_next;
    }

    p_entry->p_next = (resource_entry_t *)malloc(sizeof(resource_entry_t));

    p_entry->p_next->p_next = NULL;
    p_entry->p_next->p_prev = p_entry;

    return p_entry->p_next;
}

static void __vulkan_resource_mgr_delete_entry(resource_entry_t *p_entry)
{
    free(p_entry);

    p_entry->p_prev->p_next = NULL;
}

static void __vulkan_resource_mgr_init_resource_list(resource_list_t *p_list)
{
    p_list->name = "first";
    p_list->p_data = NULL;
    p_list->p_next = NULL;
    p_list->p_prev = NULL;

    __vulkan_resource_mgr_create_new_entry(p_list);

    return;
}

void vulkan_resource_mgr_init(void)
{
    __vulkan_resource_mgr_init_resource_list(&resource_ctx.image_list);

    __vulkan_resource_mgr_init_resource_list(&resource_ctx.buffer_list);

    __vulkan_resource_mgr_init_resource_list(&resource_ctx.vertex_list);

    return;
}

static uint32_t __vulkan_resource_mgr_register_name(resource_list_t *p_list, char *name)
{
    resource_entry_t *p_entry;

    p_entry = p_list->p_next;

    while (p_entry->p_next != NULL) {
        if (!strcmp(p_entry->name, name)) {
            printf("resource name already used: %s\n");

            return FAILURE;
        }

        p_entry = p_entry->p_next;
    }

    p_entry->name = malloc(strlen(name));

    strcpy(p_entry->name, name);

    return SUCCESS;
}

static resource_t *__vulkan_resource_mgr_copy_resource_buf(resource_data_t *p_data)
{
    uint32_t data_size;
    resource_t *p_data_buf;

    switch (p_data->type) {
        case RESOURCE_FORMAT_TYPE_VERTEX_2D_RGB:
            data_size = sizeof(vertex_2d_rgb_t) * p_data->count;
            p_data_buf = (vertex_2d_rgb_t *)malloc(data_size);
            break;

        default:
            printf("no matching format for resource %s\n", p_data->data_name);
            return NULL;
    }

    memcpy(p_data_buf, p_data, data_size);

    return p_data_buf;
}

uint32_t vulkan_resource_mgr_create_vertex_buffer(resource_data_t *p_resource)
{
    uint32_t res;
    resource_entry_t *p_entry;

    p_entry = __vulkan_resource_mgr_create_new_entry(&resource_ctx.vertex_list);

    res = __vulkan_resource_mgr_register_name(&resource_ctx.vertex_list, p_resource->data_name);
    if (res == FAILURE) {
        __vulkan_resource_mgr_delete_entry(p_entry);
        return FAILURE;
    }

    p_entry->p_data = __vulkan_resource_mgr_copy_resource_buf(p_resource);
    if (p_entry->p_data == NULL) {
        __vulkan_resource_mgr_delete_entry(p_entry);
        return FAILURE;
    }

    return SUCCESS;
}
