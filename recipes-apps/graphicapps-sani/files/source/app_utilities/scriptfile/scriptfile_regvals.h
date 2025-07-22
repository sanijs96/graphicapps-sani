#ifndef __SCRIPTFILE_REGVALS_H__
#define __SCRIPTFILE_REGVALS_H__

#include <stdio.h>
#include <stdint.h>

#define MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES   (100)

uint32_t scriptfile_allocate_regval_entry(char *argname_str, FILE *p_fstream);
uint32_t scriptfile_save_command_regval_result(char *argname_str, uint32_t retval, FILE *p_fstream);

uint32_t scriptfile_get_regval_entry_idx(char *argname_str);
char *scriptfile_get_regval_entry_name(char *argname_str);
char *scriptfile_get_regval_entry_value(char *argname_str);

void scriptfile_release_entries(FILE *p_fstream);

#endif
