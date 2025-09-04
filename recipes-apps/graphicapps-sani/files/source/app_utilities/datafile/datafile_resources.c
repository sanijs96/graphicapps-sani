#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "vulkan/resource_formats.h"

#include "datafile_resources.h"

enum resource_data_state {
    RESOURCE_DATA_STATE_DEFAULT,
    RESOURCE_DATA_STATE_ALLOCATED,
};

typedef struct resource_data {
    uint32_t state;
    uint32_t member_type;
    uint32_t member_count;
    char name[MAX_RESOURCE_NAME_LENGTH];
    resource_member_list_t *member_list;
} resource_data_t;

static struct {
    uint32_t resource_count;
    resource_data_t resource_data[MAX_NUM_RESOURCE_OBJECTS];
} datafile_ctx = { .resource_count = 0 };

const char * resource_type_names[] = {
    [RESOURCE_FORMAT_TYPE_VERTEX_BUFFER_START] = NULL,
    [RESOURCE_FORMAT_TYPE_VERTEX_BUFFER_2D_RGB] = "vertex_2d_rgb",

    [RESOURCE_FORMAT_TYPE_INDEX_BUFFER_START] = NULL,
    [RESOURCE_FORMAT_TYPE_INDEX_BUFFER_3V] = "index_3v",

    [RESOURCE_FORMAT_TYPE_IMAGE_START] = NULL,
};

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str)
{
    uint32_t list_size;
    uint32_t name_strlen;

    list_size = sizeof(resource_type_names) / sizeof(char *);

    for (uint32_t idx = 0; idx < list_size; idx++) {
        if (resource_type_names[idx] == NULL) {
            continue;
        }

        name_strlen = strlen(resource_type_names[idx]);
        if (strncmp(p_typename_str, resource_type_names[idx], name_strlen)) {
            continue;
        }

        return idx;
    }

    printf("typename %s not recognized\n", p_typename_str);

    return RESOURCE_FORMAT_TYPE_INVALID;
}

uint32_t __datafile_get_available_regval_entry(void)
{
    for (uint32_t idx = 0; idx < MAX_NUM_RESOURCE_OBJECTS; idx++) {
        if (datafile_ctx.resource_data[idx].state == RESOURCE_DATA_STATE_ALLOCATED) {
            continue;
        }

        return idx;
    }

    return MAX_NUM_RESOURCE_OBJECTS;
}

uint32_t datafile_allocate_resource_data_entry(uint32_t entry_count)
{
    uint32_t entry_idx;
    resource_data_t *p_entry;

    if (datafile_ctx.resource_count == MAX_NUM_RESOURCE_OBJECTS) {
        printf("resource entries fully allocated\n");
        return MAX_NUM_RESOURCE_OBJECTS;
    }

    entry_idx = __datafile_get_available_regval_entry();

    p_entry = &datafile_ctx.resource_data[entry_idx];

    memset(p_entry, 0, sizeof(resource_data_t));

    p_entry->state = RESOURCE_DATA_STATE_ALLOCATED;

    p_entry->member_list = (resource_member_list_t *)malloc(entry_count *
                                                            sizeof(resource_member_list_t));

    datafile_ctx.resource_count++;

    return entry_idx;
}

uint32_t datafile_get_total_resource_count(void)
{
    return datafile_ctx.resource_count;
}

uint32_t datafile_get_resource_entry_idx(char *p_argname_str)
{
    for (uint32_t idx = 0; idx < MAX_NUM_RESOURCE_OBJECTS; idx++) {
        if (strcmp(datafile_ctx.resource_data[idx].name, p_argname_str)) {
            continue;
        }

        return idx; 
    }

    printf("resource entry %s not detected\n", p_argname_str);

    return MAX_NUM_RESOURCE_OBJECTS;
}

uint32_t datafile_get_resource_name(uint32_t entry_idx, char *resource_name_buf)
{
    if (entry_idx > datafile_ctx.resource_count) {
        return FAILURE;
    }

    strcpy(resource_name_buf, datafile_ctx.resource_data[entry_idx].name);

    return SUCCESS;
}

uint32_t datafile_get_resource_type(char *resource_name)
{
    uint32_t entry_idx;

    entry_idx = datafile_get_resource_entry_idx(resource_name);
    if (entry_idx == MAX_NUM_RESOURCE_OBJECTS) {
        return RESOURCE_FORMAT_TYPE_INVALID;
    }

    return datafile_ctx.resource_data[entry_idx].member_type;
}

uint32_t datafile_get_resource_member_count(char *resource_name)
{
    uint32_t entry_idx;

    entry_idx = datafile_get_resource_entry_idx(resource_name);
    if (entry_idx == MAX_NUM_RESOURCE_OBJECTS) {
        return 0;
    }

    return datafile_ctx.resource_data[entry_idx].member_count;
}

void datafile_save_resource_name(uint32_t entry_idx, char *resource_name)
{
    strcpy(datafile_ctx.resource_data[entry_idx].name, resource_name);
}

void datafile_save_resource_type(uint32_t entry_idx, uint32_t resource_type)
{
    datafile_ctx.resource_data[entry_idx].member_type = resource_type;
}

void datafile_save_resource_count(uint32_t entry_idx, uint32_t resource_count)
{
    datafile_ctx.resource_data[entry_idx].member_count = resource_count;
}

uint32_t datafile_save_resource_data(uint32_t entry_idx,
                                        resource_member_list_t input_data,
                                            uint32_t entry_count_per_member,
                                            uint32_t current_member_list_idx)
{
    resource_data_t *p_entry;
    resource_member_list_t member_list;

    p_entry = &datafile_ctx.resource_data[entry_idx];

    p_entry->member_list[current_member_list_idx] =
                (resource_member_list_t)malloc(entry_count_per_member *
                                        sizeof(resource_member_entry_t));

    member_list = p_entry->member_list[current_member_list_idx];

    for (uint32_t idx = 0; idx < entry_count_per_member; idx++) {
        strcpy(member_list[idx].value, input_data[idx].value);
    }

    return SUCCESS;
}

resource_member_list_t *datafile_get_resource_data(char *resource_name)
{
    uint32_t entry_idx;
    resource_data_t *p_entry;

    entry_idx = datafile_get_resource_entry_idx(resource_name);
    if (entry_idx == MAX_NUM_RESOURCE_OBJECTS) {
        printf("resource not registered\n");
        return NULL;
    }

    return datafile_ctx.resource_data[entry_idx].member_list;
}

void datafile_release_entry(FILE *p_fstream)
{

}
