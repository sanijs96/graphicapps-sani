#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "datafile_resources.h"

enum resource_entry_state {
    RESOURCE_ENTRY_STATE_DEFAULT,
    RESOURCE_ENTRY_STATE_ALLOCATED,
};

typedef struct {
    char value[MAX_RESOURCE_VALUE_LENGTH];
} member_value_entry_t, *member_value_list_t;

typedef struct resource_entry {
    uint32_t state;
    resource_info_t info;
    member_value_list_t *member_list;
} resource_entry_t;

static struct {
    uint32_t resource_count;
    resource_entry_t resources[MAX_NUM_DATAFILE_RESOURCE_ENTRIES];
} datafile_ctx = { .resource_count = 0 };

typedef uint32_t (*resource_setup_fn)(resource_t *p_resource_buf, resource_entry_t *p_entry);

typedef const struct resource_handler {
    char *name;
    uint32_t usage_flags;
    resource_setup_fn fn;
} resource_handler_t;

static uint32_t __datafile_setup_resource_type_vertex_2d_rgb(resource_t *p_buf,
                                                             resource_entry_t *p_entry);

const resource_handler_t resource_handlers[] = {
    [RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_START] = {.name = NULL},
    [RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_2D_RGB] = {
        .name = "vertex_2d_rgb",
        .usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .fn = __datafile_setup_resource_type_vertex_2d_rgb,
    },
    [RESOURCE_FORMAT_TYPE_BUFFER_START] = {.name = NULL},


    [RESOURCE_FORMAT_TYPE_IMAGE_START] = {.name = NULL},
};

static uint32_t __datafile_setup_resource_type_vertex_2d_rgb(resource_t *p_buf,
                                                                resource_entry_t *p_entry)
{
    uint32_t member_idx;
    char *p_value_str;

    for (uint32_t idx = 0; idx < p_entry->info.count; idx++) {
        member_idx = 0;

        p_buf[idx].vertex_2d_rgb.pos.x = atof(p_entry->member_list[idx][member_idx++].value);
        p_buf[idx].vertex_2d_rgb.pos.y = atof(p_entry->member_list[idx][member_idx++].value);

        p_buf[idx].vertex_2d_rgb.color.r = atof(p_entry->member_list[idx][member_idx++].value);
        p_buf[idx].vertex_2d_rgb.color.g = atof(p_entry->member_list[idx][member_idx++].value);
        p_buf[idx].vertex_2d_rgb.color.b = atof(p_entry->member_list[idx][member_idx++].value);
    }

    return SUCCESS;
}

uint32_t datafile_get_resource_usage_flags_from_typename(uint32_t type)
{
    uint32_t list_size;
    uint32_t name_strlen;

    list_size = sizeof(resource_handlers) / sizeof(resource_handler_t);

    if (list_size < type) {
        return 0;
    }

    return resource_handlers[type].usage_flags;
}

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str)
{
    uint32_t list_size;
    uint32_t name_strlen;

    list_size = sizeof(resource_handlers) / sizeof(resource_handler_t);

    for (uint32_t idx = 0; idx < list_size; idx++) {
        if (resource_handlers[idx].name == NULL) {
            continue;
        }

        name_strlen = strlen(resource_handlers[idx].name);
        if (strncmp(p_typename_str, resource_handlers[idx].name, name_strlen)) {
            continue;
        }

        return idx;
    }

    printf("typename %s not recognized\n", p_typename_str);

    return RESOURCE_FORMAT_TYPE_INVALID;
}

uint32_t __datafile_get_available_regval_entry(void)
{
    for (uint32_t idx = 0; idx < MAX_NUM_DATAFILE_RESOURCE_ENTRIES; idx++) {
        if (datafile_ctx.resources[idx].state == RESOURCE_ENTRY_STATE_ALLOCATED) {
            continue;
        }

        return idx;
    }

    return MAX_NUM_DATAFILE_RESOURCE_ENTRIES;
}

uint32_t datafile_allocate_resource_entry(resource_info_t *p_info)
{
    uint32_t entry_idx;
    resource_entry_t *p_entry;

    if (datafile_ctx.resource_count == MAX_NUM_DATAFILE_RESOURCE_ENTRIES) {
        printf("resource entries fully allocated\n");
        return FAILURE;
    }

    entry_idx = __datafile_get_available_regval_entry();

    p_entry = &datafile_ctx.resources[entry_idx];

    memset(p_entry, 0, sizeof(resource_entry_t));

    p_entry->state = RESOURCE_ENTRY_STATE_ALLOCATED;

    memcpy(&p_entry->info, p_info, sizeof(resource_info_t));

    p_entry->member_list = (member_value_list_t *)malloc(sizeof(member_value_list_t) *
                                                                    p_entry->info.count);

    datafile_ctx.resource_count++;

    return SUCCESS;
}

uint32_t datafile_get_resource_entry_idx(char *p_argname_str)
{
    for (uint32_t idx = 0; idx < MAX_NUM_DATAFILE_RESOURCE_ENTRIES; idx++) {
        if (strcmp(datafile_ctx.resources[idx].info.name, p_argname_str)) {
            continue;
        }

        return idx; 
    }

    return MAX_NUM_DATAFILE_RESOURCE_ENTRIES;
}

uint32_t datafile_save_resource_values(resource_info_t *p_info,
                                        char p_value_str[][MAX_RESOURCE_VALUE_LENGTH],
                                            uint32_t value_count, uint32_t member_idx)
{
    uint32_t entry_idx;
    resource_entry_t *p_entry;
    member_value_list_t p_member_list;

    entry_idx = datafile_get_resource_entry_idx(p_info->name);
    p_entry = &datafile_ctx.resources[entry_idx];

    p_entry->member_list[member_idx] = (member_value_list_t)malloc(value_count *
                                                                    sizeof(member_value_entry_t));

    p_member_list = p_entry->member_list[member_idx];

    for (uint32_t idx = 0; idx < value_count; idx++) {
        strcpy(p_member_list[idx].value, p_value_str[idx]);
    }

    return SUCCESS;
}

resource_info_t *datafile_get_resource_info(char *p_argname_str)
{
    uint32_t entry_idx;
    entry_idx = datafile_get_resource_entry_idx(p_argname_str);

    if (entry_idx == MAX_NUM_DATAFILE_RESOURCE_ENTRIES) {
        printf("resource not registered\n");
        return NULL;
    }

    return &datafile_ctx.resources[entry_idx].info;
}

static resource_handler_t *__datafile_get_resource_handler(uint32_t type)
{
    uint32_t list_size;

    list_size = sizeof(resource_handlers) / sizeof(resource_handler_t);
    if (type > list_size) {
        printf("resource handler not detected\n");
        return NULL;
    }

    return &resource_handlers[type];
}

uint32_t datafile_get_resource_data(resource_t *p_resource_buf, resource_info_t *p_info)
{
    uint32_t entry_idx;
    resource_entry_t *p_entry;
    resource_handler_t *p_handler;

    entry_idx = datafile_get_resource_entry_idx(p_info->name);
    if (entry_idx == MAX_NUM_DATAFILE_RESOURCE_ENTRIES) {
        printf("resource not registered\n");
        return FAILURE;
    }

    p_entry = &datafile_ctx.resources[entry_idx];

    p_handler = __datafile_get_resource_handler(p_info->type);
    if (p_handler == NULL) {
        return FAILURE;
    }

    if (p_handler->fn(p_resource_buf, p_entry) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

void datafile_release_entry(FILE *p_fstream)
{

}


