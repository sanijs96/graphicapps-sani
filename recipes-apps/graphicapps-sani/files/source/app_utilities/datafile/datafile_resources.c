#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "vulkan/resource_formats.h"

#include "datafile_resources.h"

enum resource_entry_state {
    RESOURCE_ENTRY_STATE_DEFAULT,
    RESOURCE_ENTRY_STATE_ALLOCATED,
};

typedef struct resource_entry {
    uint32_t state;
    char name[MAX_RESOURCE_NAME_LENGTH];
    resource_member_list_t *member_list;
} resource_data_t;

static struct {
    uint32_t resource_count;
    resource_data_t resources[MAX_NUM_RESOURCE_OBJECTS];
} datafile_ctx = { .resource_count = 0 };

typedef const struct resource_type_identifier_t {
    char *name;
    uint32_t usage_flags;
} resource_type_identifier_t;

const resource_type_identifier_t resource_type_identifiers[] = {
    [RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_START] = {NULL, 0},
    [RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_2D_RGB] = {"vertex_2d_rgb",
                                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT},
};

uint32_t datafile_get_usage_flags_from_resource_type(uint32_t type)
{
    uint32_t list_size;
    uint32_t name_strlen;

    list_size = sizeof(resource_type_identifiers) / sizeof(resource_type_identifier_t);

    if (list_size < type) {
        return 0;
    }

    return resource_type_identifiers[type].usage_flags;
}

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str)
{
    uint32_t list_size;
    uint32_t name_strlen;

    list_size = sizeof(resource_type_identifiers) / sizeof(resource_type_identifier_t);

    for (uint32_t idx = 0; idx < list_size; idx++) {
        if (resource_type_identifiers[idx].name == NULL) {
            continue;
        }

        name_strlen = strlen(resource_type_identifiers[idx].name);
        if (strncmp(p_typename_str, resource_type_identifiers[idx].name, name_strlen)) {
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
        if (datafile_ctx.resources[idx].state == RESOURCE_ENTRY_STATE_ALLOCATED) {
            continue;
        }

        return idx;
    }

    return MAX_NUM_RESOURCE_OBJECTS;
}

uint32_t datafile_allocate_resource_data_entry(resource_info_t *p_info)
{
    uint32_t entry_idx;
    resource_data_t *p_entry;

    if (datafile_ctx.resource_count == MAX_NUM_RESOURCE_OBJECTS) {
        printf("resource entries fully allocated\n");
        return FAILURE;
    }

    entry_idx = __datafile_get_available_regval_entry();

    p_entry = &datafile_ctx.resources[entry_idx];

    memset(p_entry, 0, sizeof(resource_data_t));

    p_entry->state = RESOURCE_ENTRY_STATE_ALLOCATED;

    strcpy(p_entry->name, p_info->name);

    p_entry->member_list = (resource_member_list_t *)malloc(sizeof(resource_member_list_t) *
                                                                                p_info->count);

    datafile_ctx.resource_count++;

    return SUCCESS;
}

uint32_t datafile_get_resource_entry_idx(char *p_argname_str)
{
    for (uint32_t idx = 0; idx < MAX_NUM_RESOURCE_OBJECTS; idx++) {
        if (strcmp(datafile_ctx.resources[idx].name, p_argname_str)) {
            continue;
        }

        return idx; 
    }

    return MAX_NUM_RESOURCE_OBJECTS;
}

uint32_t datafile_save_resource_data(resource_info_t *p_info,
                                        resource_member_list_t member_list_input,
                                        uint32_t value_count, uint32_t member_idx)
{
    uint32_t entry_idx;
    resource_data_t *p_entry;
    resource_member_list_t member_list;

    entry_idx = datafile_get_resource_entry_idx(p_info->name);
    p_entry = &datafile_ctx.resources[entry_idx];

    p_entry->member_list[member_idx] = (resource_member_list_t)malloc(value_count *
                                                                sizeof(resource_member_entry_t));

    member_list = p_entry->member_list[member_idx];

    for (uint32_t idx = 0; idx < value_count; idx++) {
        strcpy(member_list[idx].value, member_list_input[idx].value);
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

    return datafile_ctx.resources[entry_idx].member_list;
}

void datafile_release_entry(FILE *p_fstream)
{

}
