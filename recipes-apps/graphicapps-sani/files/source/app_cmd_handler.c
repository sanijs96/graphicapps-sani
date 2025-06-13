#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "app_cmd_handler.h"

#include "vulkan_obj_mgr/vulkan_obj_mgr_cmd.h"

#include "etc_app_cmd.h"

typedef const struct __command_handler_entry {
    const char * name;
    const char * args_list;

    uint32_t (*check_sanity)(command_t *p_cmd);
    uint32_t (*process)(command_t *p_cmd);
    uint32_t (*usage)(command_t *p_cmd);
} const command_handler_entry_t;

command_handler_entry_t vulkan_cmd_handler_instance[] = {
    {"create", NULL,
        NULL, _vulkan_obj_mgr_cmd_create_instance, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_cmd_handler_layer[] = {
    {"add", "ns", NULL, _vulkan_obj_mgr_cmd_enable_layer, NULL},
    {"del", "ns" , NULL, _vulkan_obj_mgr_cmd_disable_layer, NULL},
    {"list", NULL, NULL, _vulkan_obj_mgr_cmd_show_layers_list, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_cmd_handler_extension[] = {
    {"add", "ns", NULL, _vulkan_obj_mgr_cmd_enable_extension, NULL},
    {"del", "ns", NULL, _vulkan_obj_mgr_cmd_disable_extension, NULL},
    {"list", NULL, NULL, _vulkan_obj_mgr_cmd_show_extensions_list, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_cmd_handler_device[] = {
    {"create", "i", NULL, _vulkan_obj_mgr_cmd_create_device, NULL},
    {"list", NULL, NULL, _vulkan_obj_mgr_cmd_show_devices_list, NULL},

    {NULL, }
};

command_handler_entry_t etc_cmd_handler_window[] = {
    {"resize", "wh", NULL, NULL, NULL},

    {NULL, }
};

command_handler_entry_t etc_cmd_handler_console[] = {
    {"exit", NULL, NULL, NULL, NULL},

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
    while (p_entry->name) {
        if (!strcmp(p_entry->name, subcmd_name)) {
            return p_entry;
        }
        p_entry++;
    }

    return NULL;
}

static command_handler_entry_t *__app_cmd_find_cmd_handler_entry(command_t *p_cmd)
{
    command_handler_entry_t *p_entry;

    p_entry = __find_matching_cmd_entry_list(app_cmd_list, p_cmd->cmd_name);

    if (p_entry) {
        return __find_matching_cmd_entry(p_entry, p_cmd->subcmd_name);
    }

    return NULL;
}

uint32_t app_cmd_check_max_num_cmd_args(command_t *p_cmd)
{
    command_handler_entry_t *p_entry;
    p_entry = __app_cmd_find_cmd_handler_entry(p_cmd);

    if (p_entry && p_entry->args_list) {
        return strlen(p_entry->args_list);
    }
    else {
        return 0;
    }
}

uint32_t app_cmd_process(command_t *p_cmd)
{
    uint32_t res;
    command_handler_entry_t *p_cmd_entry;

    p_cmd_entry = __app_cmd_find_cmd_handler_entry(p_cmd);
    if (!p_cmd_entry) {
        res = FAILURE;
        goto exit;
    }

    if (p_cmd_entry->check_sanity) {
        res = p_cmd_entry->check_sanity(p_cmd);

        if (res == FAILURE) {
            goto exit;
        }
    }

    if (p_cmd_entry->process) {
        res = p_cmd_entry->process(p_cmd);
    }
    else {
        printf("command not implemented..\n");
        res = FAILURE;
    }

exit:
    return res;
}

uint32_t app_cmd_check_exited(command_t *p_cmd)
{
    if (!strcmp(p_cmd->cmd_name, "console") &&
        !strcmp(p_cmd->subcmd_name, "exit")) {
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

    if (!p_cmd_entry || !p_cmd_entry->usage) {
        return;
    }

    p_cmd_entry->usage(p_cmd);

    return;
}
