#ifndef __DATAFILE_REGOBJS_H__
#define __DATAFILE_REGOBJS_H__

#include <stdio.h>
#include <stdint.h>

#include "vulkan/resource_formats.h"

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str);
uint32_t datafile_get_usage_flags_from_resource_type(uint32_t type);

uint32_t datafile_allocate_resource_data_entry(resource_info_t *p_info);

uint32_t datafile_get_resource_entry_idx(char *p_argname_str);

uint32_t datafile_save_resource_data(resource_info_t *p_info,
                                        resource_member_list_t member_list_input,
                                        uint32_t value_count, uint32_t member_idx);

resource_member_list_t *datafile_get_resource_data(char *resource_name);

void datafile_release_entry(FILE *p_fstream);

#endif
