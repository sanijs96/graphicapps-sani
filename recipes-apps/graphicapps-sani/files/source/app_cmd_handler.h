#ifndef __APPS_CMD_H__
#define __APPS_CMD_H__

#include <stdint.h>
#include "app/app_cmd_def.h"

uint32_t app_cmd_setup_cmd_handler(command_t *p_cmd);

void app_cmd_parse_args(command_t *p_cmd);

uint32_t app_cmd_check_sanity(command_t *p_cmd);

uint32_t app_cmd_check_exited(command_t *p_cmd);

uint32_t app_cmd_process(command_t *p_cmd);

void app_cmd_show_usage(command_t *p_cmd);

void app_cmd_show_result(command_t *p_cmd);

#endif
