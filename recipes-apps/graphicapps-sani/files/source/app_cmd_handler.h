#ifndef __APPS_CMD_H__
#define __APPS_CMD_H__

#include "app/app_cmd_def.h"

uint32_t app_cmd_handler_check_exited(command_t *p_cmd);

uint32_t app_cmd_handler_check_argname_registered(char *arg_name_str);
void app_cmd_handler_convert_argname_to_argval(char *arg_name_str);
uint32_t app_cmd_handler_save_result(char *arg_name_str, uint32_t retval);

uint32_t app_cmd_handler_process(command_t *p_cmd);

void app_cmd_handler_show_usage(command_t *p_cmd);

uint32_t app_cmd_handler_check_max_num_cmd_args(command_t *p_cmd);

uint32_t app_cmd_handler_get_command_input(char *cmd_string);

#endif
