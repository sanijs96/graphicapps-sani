#ifndef __DATASCRIPT_PARSER_H__
#define __DATASCRIPT_PARSER_H__

#include <stdint.h>

#include "vulkan/resource_formats.h"

uint32_t datascript_register_scriptfile(char *filename_str);

uint32_t datascript_check_scriptfile_registered(void);

uint32_t datascript_get_command(char *input_str);

uint32_t datascript_save_command_result(char *argname_str, uint32_t retval);

uint32_t datascript_register_datafile(char *filename_str);

uint32_t datascript_get_total_resource_count(void);

uint32_t datascript_load_all_resource_objects(void);

uint32_t datascript_get_resource_name(uint32_t resource_entry_idx, char *resource_name_buf);

uint32_t datascript_get_resource_type(char *resource_name);

uint32_t datascript_get_resource_member_count(char *resource_name);

uint32_t datascript_check_resource_registered(char *resource_name);

uint32_t datascript_get_resource_entry_idx(char *resource_name);

resource_member_list_t *datascript_get_resource_data(char *resource_name);

void datascript_release_all_files(void);

#endif
