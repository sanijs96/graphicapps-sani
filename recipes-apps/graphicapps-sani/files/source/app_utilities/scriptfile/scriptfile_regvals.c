#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "common/common_def.h"

#include "scriptfile_regvals.h"

#define MAX_REGVAL_VALUE_LENGTH     (10)
#define MAX_REGVAL_NAME_LENGTH      (MAX_LENGTH_ARGUMENT_NAME)

enum regval_entry_stats {
    REGVAL_ENTRY_STATE_DEFAULT,
    REGVAL_ENTRY_STATE_ALLOCATED,
};

typedef struct regval_entry {
    uint32_t state;
    char name[MAX_REGVAL_NAME_LENGTH];
    char value[MAX_REGVAL_VALUE_LENGTH];
    FILE* p_fstream_scope;
} regval_entry_t;

static struct {
    uint32_t regval_count;
    regval_entry_t regvals[MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES];
} scriptfile_ctx = { .regval_count = 0 };

uint32_t __scriptfile_get_available_regval_entry(void)
{
    for (uint32_t idx = 0; idx < MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES; idx++) {
        if (scriptfile_ctx.regvals[idx].state == REGVAL_ENTRY_STATE_ALLOCATED) {
            continue;
        }

        return idx;
    }

    return MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES;
}

uint32_t scriptfile_save_command_regval_result(char *argname_str, uint32_t retval, FILE *p_fstream)
{
    uint32_t regval_entry_idx;
    regval_entry_t *p_entry;

    regval_entry_idx = __scriptfile_get_available_regval_entry();
    if (regval_entry_idx == MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES) {
        printf("too many arguments registered\n");
        return FAILURE;
    }

    p_entry = &scriptfile_ctx.regvals[regval_entry_idx];

    memset(p_entry->name, 0, MAX_REGVAL_NAME_LENGTH);
    memset(p_entry->value, 0, MAX_REGVAL_VALUE_LENGTH);

    strncpy(p_entry->name, argname_str, strlen(argname_str));
    p_entry->name[strlen(argname_str)] = '\0';

    sprintf(p_entry->value, "%u", retval);

    scriptfile_ctx.regvals[regval_entry_idx].p_fstream_scope = p_fstream;

    scriptfile_ctx.regvals[regval_entry_idx].state = REGVAL_ENTRY_STATE_ALLOCATED;

    scriptfile_ctx.regval_count++;

    return SUCCESS;
}

uint32_t scriptfile_get_regval_entry_idx(char *p_argame_str)
{
    regval_entry_t *p_entries;

    p_entries = scriptfile_ctx.regvals;

    for (uint32_t idx = 0; idx < scriptfile_ctx.regval_count; idx++) {
        if (strncmp(p_entries[idx].name, p_argame_str, strlen(p_entries[idx].name))) {
            continue;
        }

        return idx;
    }

    printf("regval entry fully allocated\n");

    return MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES;
}

char *scriptfile_get_regval_entry_name(char *p_argname_str)
{
    uint32_t regval_idx;

    regval_idx = scriptfile_get_regval_entry_idx(p_argname_str);
    if (regval_idx == MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES) {
        return NULL;
    }

    return scriptfile_ctx.regvals[regval_idx].name;
}

char *scriptfile_get_regval_entry_value(char *p_argname_str)
{
    uint32_t regval_idx;

    regval_idx = scriptfile_get_regval_entry_idx(p_argname_str);
    if (regval_idx == MAX_NUM_SCRIPTFILE_REGVAL_ENTRIES) {
        return NULL;
    }

    return scriptfile_ctx.regvals[regval_idx].value;
}

void scriptfile_release_entries(FILE *p_fstream)
{
    uint32_t regval_idx;
    uint32_t regval_count;
    regval_entry_t *p_entry;

    regval_count = scriptfile_ctx.regval_count;

    for (uint32_t idx = 0; idx < regval_count; idx++) {
        p_entry = &scriptfile_ctx.regvals[idx];

        if (p_entry->p_fstream_scope != p_fstream) {
            continue;
        }

        printf("release regval %s\n", p_entry->name);

        memset(p_entry->name, 0, MAX_REGVAL_NAME_LENGTH);
        memset(p_entry->value, 0, MAX_REGVAL_VALUE_LENGTH);

        p_entry->p_fstream_scope = NULL;
        p_entry->state = REGVAL_ENTRY_STATE_DEFAULT;
    }

    return;
}
