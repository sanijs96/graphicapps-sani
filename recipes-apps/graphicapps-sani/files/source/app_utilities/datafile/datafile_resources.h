#ifndef __DATAFILE_REGOBJS_H__
#define __DATAFILE_REGOBJS_H__

#include <stdio.h>
#include <stdint.h>

#include "vulkan/resource_formats.h"

uint32_t datafile_get_resource_type_from_typename(char *p_typename_str);

uint32_t datafile_allocate_resource_data_entry(uint32_t entry_count);

uint32_t datafile_get_total_resource_count(void);

uint32_t datafile_get_resource_entry_idx(char *p_argname_str);

void datafile_save_resource_name(uint32_t entry_idx, char *resource_name);
void datafile_save_resource_type(uint32_t entry_idx, uint32_t resource_type);
void datafile_save_resource_count(uint32_t entry_idx, uint32_t resource_count);

uint32_t datafile_get_resource_name(uint32_t entry_idx, char *resource_name_buf);
uint32_t datafile_get_resource_type(char *resource_name);
uint32_t datafile_get_resource_member_count(char *resource_name);
uint32_t datafile_save_resource_data(uint32_t entry_idx,
                                        resource_member_list_t input_data,
                                            uint32_t entry_count_per_member,
                                            uint32_t current_member_list_idx);

resource_member_list_t *datafile_get_resource_data(char *resource_name);

void datafile_release_entry(FILE *p_fstream);

#endif
