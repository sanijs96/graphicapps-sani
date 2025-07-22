#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "datafile_resources.h"

#define MAX_RESOURCE_NAME_LENGTH        (MAX_LENGTH_ARGUMENT_NAME)

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

const char * const resource_type_names_list [NUM_RESOURCE_FORMAT_TYPES] = {
    [RESOURCE_FORMAT_TYPE_VERTEX_2D_RGB] = "vertex_2d_rgb",
};

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str)
{
    uint32_t name_strlen;

    for (uint32_t idx = 0; idx < NUM_RESOURCE_FORMAT_TYPES; idx++) {
        name_strlen = strlen(resource_type_names_list[idx]);
        if (strncmp(p_typename_str, resource_type_names_list[idx], name_strlen)) {
            continue;
        }

        return idx;
    }

    printf("typename %s not recognized\n", p_typename_str);

    return NUM_RESOURCE_FORMAT_TYPES;
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

    memcpy(p_entry, p_info, sizeof(resource_info_t));

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
    member_value_entry_t *p_member_entry;

    entry_idx = datafile_get_resource_entry_idx(p_info->name);
    p_entry = &datafile_ctx.resources[entry_idx];

    p_entry->member_list[member_idx] = (member_value_list_t)malloc(value_count *
                                                                    sizeof(member_value_entry_t));

    for (uint32_t idx = 0; idx < value_count; idx++) {
        p_member_entry = &p_entry->member_list[member_idx];
        strcpy(p_member_entry, p_value_str[idx]);
    }

    return SUCCESS;
}

resource_info_t *datafile_get_resource_info(char *p_argname_str)
{

}

uint32_t datafile_get_resource_object(resource_t *p_resource_buf)
{

}

void datafile_release_entry(FILE *p_fstream)
{

}


