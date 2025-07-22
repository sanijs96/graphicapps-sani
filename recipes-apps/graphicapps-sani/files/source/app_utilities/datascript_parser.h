#ifndef __DATASCRIPT_PARSER_H__
#define __DATASCRIPT_PARSER_H__

#include <stdint.h>

#include "vulkan/resource_formats.h"

uint32_t datascript_register_scriptfile(char *filename_str);
uint32_t datascript_check_scriptfile_registered(void);
uint32_t datascript_get_command(char *input_str);
uint32_t datascript_save_command_result(char *argname_str, uint32_t retval);

uint32_t datascript_register_datafile(char *filename_str);
uint32_t datascript_check_resource_registered(char *argname_str);
uint32_t datascript_load_resources_from_datafile(char *filename_str);
resource_info_t *datascript_get_resource_info(char *argname_str);
uint32_t datascript_get_resource_object(resource_t *resource_buf, char *argname_str);

void datascript_release_all_files(void);

#endif
