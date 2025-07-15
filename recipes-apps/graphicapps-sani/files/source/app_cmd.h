#ifndef __APP_CMD_H__
#define __APP_CMD_H__

#include "app/app_cmd_def.h"

enum app_cmd_runscript_state {
    APP_CMD_RUNSCRIPT_DEFAULT = 0,
    APP_CMD_RUNSCRIPT_REGISTERED,
    APP_CMD_RUNSCRIPT_FINISHED,
};

uint32_t app_cmd_add_runscript_file(command_t *p_cmd);
uint32_t app_cmd_get_runscript_state(void);
uint32_t app_cmd_get_cmdstring_from_runscript(char **p_input_str);

uint32_t app_cmd_check_argname_registered(char *input_str);
char *app_cmd_load_argval_from_argname(char *arg_name);
uint32_t app_cmd_save_result(char *arg_name, uint32_t retval);

uint32_t app_cmd_console_exit(command_t *p_cmd);

#endif
