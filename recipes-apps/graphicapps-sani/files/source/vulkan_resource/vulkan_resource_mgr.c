#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "vulkan_resource_mgr.h"

typedef struct resource_entry {
    void *p_data_buf;
    char *resource_name;
    struct resource_entry *p_next;
} resource_entry_t, *resource_list_t;

static resource_list_t image_list = NULL;
static resource_list_t buffer_list = NULL;
static resource_list_t vertex_list = NULL;

static resource_entry_t *__vulkan_resource_mgr_create_new_entry(resource_list_t p_list)
{
    resource_entry_t *p_entry;

    p_entry = p_list;

    while (p_entry->p_next != NULL) {
        p_entry = p_entry->p_next;
    }

    p_entry->p_next = (resource_entry_t *)malloc(sizeof(resource_entry_t));

    p_entry->p_next->p_next = NULL;

    return p_entry->p_next;
}

static void __vulkan_resource_mgr_delete_entry(resource_list_t p_list, resource_entry_t *p_entry)
{
    resource_entry_t *p_cursor;

    if (p_list == NULL) {
        return;
    }

    p_cursor = p_list;

    while (p_cursor->p_next != p_entry) {
        p_cursor = p_cursor->p_next;
    }

    free(p_entry);

    p_cursor->p_next = NULL;

    return;
}

static uint32_t __vulkan_resource_mgr_register_name(resource_list_t p_list, char *name)
{
    resource_entry_t *p_entry;

    p_entry = p_list->p_next;

    while (p_entry->p_next != NULL) {
        if (!strcmp(p_entry->resource_name, name)) {
            printf("resource name already used: %s\n", p_entry->resource_name);

            return FAILURE;
        }

        p_entry = p_entry->p_next;
    }

    p_entry->resource_name = malloc(strlen(name));

    strcpy(p_entry->resource_name, name);

    return SUCCESS;
}

static void *__vulkan_resource_mgr_create_resource_buffer(resource_t *p_resources,
                                                            resource_info_t *p_info)
{
    uint8_t *p_resource_buf;
    uint32_t data_unit_size;

    switch (p_info->type) {
        case RESOURCE_FORMAT_TYPE_VERTEX_2D_RGB:
            data_unit_size = sizeof(vertex_2d_rgb_t);
            break;

        default:
            printf("no matching format for resource type %s\n", p_info->name);
            return NULL;
    }

    p_resource_buf = malloc(data_unit_size * p_info->count);

    for (uint32_t idx = 0; idx < p_info->count; idx++) {
        memcpy(&p_resource_buf[idx + data_unit_size], &p_resources[idx], data_unit_size);
    }

    return (void *)p_resource_buf;
}

uint32_t vulkan_resource_mgr_create_vertex_buffer(resource_t *p_resources, resource_info_t *p_info)
{
    uint32_t res;
    resource_entry_t *p_entry;

    p_entry = __vulkan_resource_mgr_create_new_entry(vertex_list);

    res = __vulkan_resource_mgr_register_name(vertex_list, p_info->name);
    if (res == FAILURE) {
        __vulkan_resource_mgr_delete_entry(vertex_list, p_entry);
        return FAILURE;
    }

    p_entry->p_data_buf = __vulkan_resource_mgr_create_resource_buffer(p_resources, p_info);
    if (p_entry->p_data_buf == NULL) {
        __vulkan_resource_mgr_delete_entry(vertex_list, p_entry);
        return FAILURE;
    }

    return SUCCESS;
}
