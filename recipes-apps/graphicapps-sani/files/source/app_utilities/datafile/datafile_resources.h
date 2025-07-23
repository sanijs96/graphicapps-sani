#ifndef __DATAFILE_REGOBJS_H__
#define __DATAFILE_REGOBJS_H__

#include <stdio.h>
#include <stdint.h>

#include "vulkan/resource_formats.h"

#define MAX_RESOURCE_VALUE_LENGTH           (20)
#define MAX_NUM_DATAFILE_RESOURCE_ENTRIES   (100)

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str);

uint32_t datafile_get_resource_usage_flags_from_typename(char *p_typename_str);

uint32_t datafile_allocate_resource_entry(resource_info_t *p_info);

uint32_t datafile_get_resource_entry_idx(char *p_argname_str);

uint32_t datafile_save_resource_values(resource_info_t *p_info,
                                        char p_value_str[][MAX_RESOURCE_VALUE_LENGTH],
                                            uint32_t value_count, uint32_t member_idx);

resource_info_t *datafile_get_resource_info(char *p_argname_str);

uint32_t datafile_get_resource_data(resource_t *p_resource_buf, resource_info_t *p_info);

void datafile_release_entries(FILE *p_fstream);

#endif
