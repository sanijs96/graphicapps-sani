#ifndef __ETC_APP_CMD_LIST_H__
#define __ETC_APP_CMD_LIST_H__

#include "app/app_cmd_def.h"

uint32_t etc_cmd_window_display(command_t *p_cmd);
uint32_t etc_cmd_window_resize(command_t *p_cmd);
uint32_t etc_cmd_show_window_ctx_info(command_t *p_cmd);
uint32_t etc_cmd_window_add_instance_obj(command_t *p_cmd);

uint32_t etc_cmd_console_exit(command_t *p_cmd);

#endif
