#ifndef __APP_CMD_H__
#define __APP_CMD_H__

#include "app/app_cmd_def.h"

enum app_cmd_runscript_state {
    APP_CMD_RUNSCRIPT_DEFAULT = 0,
    APP_CMD_RUNSCRIPT_REGISTERED,
    APP_CMD_RUNSCRIPT_FINISHED,
};

uint32_t app_cmd_add_runscript_file(command_t *p_cmd);

uint32_t app_cmd_load_resource_data(command_t *p_cmd);

uint32_t app_cmd_console_exit(command_t *p_cmd);

#endif
