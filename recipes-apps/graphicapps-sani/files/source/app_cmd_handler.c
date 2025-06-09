#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_obj_mgr/vulkan_obj_mgr_cmd.h"
#include "window_obj_mgr/window_obj_mgr.h"

#include "app_cmd_handler.h"

typedef const struct __command_handler_entry {
    const char * cid_entry_name;

    const uint32_t opcode;
    const char * args_list;

    uint32_t (*check_sanity)(command_t *p_cmd);
    uint32_t (*process)(command_t *p_cmd);
    uint32_t (*usage)(command_t *p_cmd);
} const command_handler_entry_t;

command_handler_entry_t vulkan_cmd_handler_instance[] = {
    {"create", OPCODE_VK(INSTANCE, CREATE), NULL, NULL, _vulkan_obj_mgr_create_instance, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_cmd_handler_layer[] = {
    {"add", OPCODE_VK(LAYER, ADD), "ns",
        NULL, _vulkan_obj_mgr_enable_layer, NULL},
    {"del", OPCODE_VK(LAYER, DEL), "ns" ,
        NULL, _vulkan_obj_mgr_disable_layer, NULL},

    {"list", OPCODE_VK(LAYER, SHOW_LIST), NULL,
        NULL, NULL, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_cmd_handler_extension[] = {
    {"add", OPCODE_VK(EXTENSION, ADD), "ns",
        NULL, _vulkan_obj_mgr_enable_extension, NULL},
    {"del", OPCODE_VK(EXTENSION, DEL), "ns",
        NULL, _vulkan_obj_mgr_disable_extension, NULL},

    {"list", OPCODE_VK(EXTENSION, SHOW_LIST), NULL ,
        NULL, NULL, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_cmd_handler_device[] = {
    {"create", OPCODE_VK(DEVICE, CREATE), NULL,
        NULL, _vulkan_obj_mgr_create_device, NULL},

    {"list", OPCODE_VK(DEVICE, SHOW_LIST), NULL,
        NULL, NULL, NULL},

    {NULL, }
};

command_handler_entry_t etc_cmd_handler_window[] = {
    {"resize", OPCODE_ETC(WINDOW, RESIZE), "wh",
        NULL, NULL, NULL},

    {NULL, }
};

command_handler_entry_t etc_cmd_handler_console[] = {
    {"exit", OPCODE_ETC(CONSOLE, EXIT), "ns",
        NULL, NULL, NULL},

    {NULL, }
};

typedef const struct __command_handler_entry_list {
    const char * cid_list_name;

    command_handler_entry_t *entries;
} const command_handler_entry_list_t;

command_handler_entry_list_t app_cmd_list[] = {
    {"instance", vulkan_cmd_handler_instance},
    {"layer", vulkan_cmd_handler_layer},
    {"extension", vulkan_cmd_handler_extension},
    {"device", vulkan_cmd_handler_device},

    {"window", etc_cmd_handler_window},
    {"console", etc_cmd_handler_console},

    {NULL, }
};

static command_handler_entry_t *__find_matching_cmd_entry_list(command_handler_entry_list_t *p_list, char *cmd_name)
{
    while (p_list->cid_list_name) {
        if (!strcmp(p_list->cid_list_name, cmd_name)) {
            return p_list->entries;
        }

        p_list++;
    }

    return NULL;
}

static command_handler_entry_t *__find_matching_cmd_entry(command_handler_entry_t *p_entry, char *subcmd_name)
{
    while (p_entry->cid_entry_name) {
        if (!strcmp(p_entry->cid_entry_name, subcmd_name)) {
            return p_entry;
        }

        p_entry++;
    }

    return NULL;
}

__always_inline static command_handler_entry_t *__app_cmd_find_cmd_handler_entry(command_t *p_cmd)
{
    char *cmd;
    char *subcmd;
    command_handler_entry_t *p_entry;

    cmd = strtok(p_cmd->input, " ");
    p_entry = __find_matching_cmd_entry_list(app_cmd_list, cmd);

    if (p_entry) {
        subcmd = strtok(NULL, " ");
        return __find_matching_cmd_entry(p_entry, subcmd);
    }

    return NULL;
}

#define __app_cmd_get_args_type()           (*(uint8_t *)strtok(NULL, "-"))
#define __app_cmd_get_args_value()          strtok(NULL, " ")

uint32_t app_cmd_process(command_t *p_cmd)
{
    uint32_t res;
    command_handler_entry_t *p_cmd_entry;

    p_cmd_entry = __app_cmd_find_cmd_handler_entry(p_cmd);

    p_cmd->opcode = p_cmd_entry->opcode;
    p_cmd->num_args = strlen(p_cmd_entry->args_list);

    command_arg_t p_cmd_arguments[p_cmd->num_args];

    for (uint32_t idx = 0; idx < p_cmd->num_args; idx++) {
        p_cmd_arguments[idx].type = __app_cmd_get_args_type();
        p_cmd_arguments[idx].value = __app_cmd_get_args_value();
    }

    res = SUCCESS;

    if (!p_cmd_entry->check_sanity) {
        res = p_cmd_entry->check_sanity(p_cmd);
    }

    if (!p_cmd_entry->process) {
        printf("command not implemented\n");
        res = FAILURE;
    }
    else {
        res = p_cmd_entry->process(p_cmd);
    }

exit:
    return res;
}

uint32_t app_cmd_check_exited(command_t *p_cmd)
{
    if (p_cmd->opcode == OPCODE_ETC(CONSOLE, EXIT)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void app_cmd_show_usage(command_t *p_cmd)
{
    command_handler_entry_t *p_cmd_entry;

    p_cmd_entry = __app_cmd_find_cmd_handler_entry(p_cmd);

    p_cmd_entry->usage(p_cmd);
}
