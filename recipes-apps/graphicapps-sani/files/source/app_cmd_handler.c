#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "app_cmd_handler.h"

#include "vulkan_app_cmd.h"
#include "etc_app_cmd.h"
#include "app_cmd.h"

typedef const struct __command_handler_entry {
    const char * name;
    const char * args_list;

    uint32_t (*preproc)(command_t *p_cmd);
    uint32_t (*process)(command_t *p_cmd);
    uint32_t (*usage)(command_t *p_cmd);
} const command_handler_entry_t;

command_handler_entry_t vulkan_app_cmd_handler_instance[] = {
    {"create", NULL,
        NULL, vulkan_app_cmd_create_instance, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_app_cmd_handler_layer[] = {
    {"add", "ns", NULL, vulkan_app_cmd_enable_layer, NULL},
    {"del", "ns" , NULL, vulkan_app_cmd_disable_layer, NULL},
    {"list", NULL, NULL, vulkan_app_cmd_show_layers_list, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_app_cmd_handler_extension[] = {
    {"add", "ins", NULL, vulkan_app_cmd_enable_extension, NULL},
    {"del", "ins", NULL, vulkan_app_cmd_disable_extension, NULL},
    {"list", NULL, NULL, vulkan_app_cmd_show_extensions_list, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_app_cmd_handler_device[] = {
    {"create", "i", NULL, vulkan_app_cmd_create_device, NULL},
    {"list", "ei", NULL, vulkan_app_cmd_show_devices_list, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_app_cmd_handler_pipeline[] = {
    {"add", "if", NULL, vulkan_app_cmd_add_pipeline_stage, NULL},
    {"create", "t", NULL, vulkan_app_cmd_create_pipeline, NULL},
    {"setup", "b", NULL, vulkan_app_cmd_setup_pipeline, NULL},
    {"run", "t", NULL, vulkan_app_cmd_run_pipeline, NULL},
    {"info", NULL, NULL, vulkan_app_cmd_show_pipeline_info, NULL},

    {NULL, }
};

command_handler_entry_t vulkan_app_cmd_handler_command_buf[] = {
    {"alloc", NULL, NULL, vulkan_app_cmd_allocate_command_buffer, NULL},
    {"add", "cis", NULL, vulkan_app_cmd_add_vulkan_command, NULL},
    {"info", "i", NULL, vulkan_app_cmd_show_command_buffer_info, NULL},

    {NULL, }
};

command_handler_entry_t etc_app_cmd_handler_window[] = {
    {"display", "i", etc_app_cmd_window_add_instance_obj, etc_app_cmd_window_display, NULL},
    {"resize", "hw", NULL, etc_app_cmd_window_resize, NULL},
    {"info", NULL, NULL, etc_app_cmd_show_window_ctx_info, NULL},

    {NULL, }
};

command_handler_entry_t app_cmd_handler_app[] = {
    {"runscript", "f", NULL, app_cmd_add_runscript_file, NULL},
    {"exit", NULL, NULL, app_cmd_console_exit, NULL},

    {NULL, }
};

typedef const struct __command_handler_entry_list {
    const char *cid_list_name;

    command_handler_entry_t *entries;
} const command_handler_entry_list_t;

command_handler_entry_list_t app_cmd_handler_list[] = {
    {"app", app_cmd_handler_app},

    {"instance", vulkan_app_cmd_handler_instance},
    {"layer", vulkan_app_cmd_handler_layer},
    {"extension", vulkan_app_cmd_handler_extension},
    {"device", vulkan_app_cmd_handler_device},
    {"pipeline", vulkan_app_cmd_handler_pipeline},
    {"cmdbuf", vulkan_app_cmd_handler_command_buf},

    {"window", etc_app_cmd_handler_window},

    {NULL, }
};

uint32_t app_cmd_handler_get_command_input(char *p_cmd_string)
{
    if (app_cmd_get_runscript_state() == APP_CMD_RUNSCRIPT_REGISTERED) {
        return app_cmd_get_cmdstring_from_runscript(p_cmd_string);
    }
    else if (!fgets(p_cmd_string, MAX_LENGTH_APP_CMD, stdin)) {
        return FAILURE;
    }
}

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

static command_handler_entry_t *__app_cmd_handler_find_cmd_handler_entry(command_t *p_cmd)
{
    command_handler_entry_t *p_entry;

    p_entry = __find_matching_cmd_entry_list(app_cmd_handler_list, p_cmd->cmd_name);

    if (p_entry) {
        return __find_matching_cmd_entry(p_entry, p_cmd->subcmd_name);
    }

    return NULL;
}

uint32_t app_cmd_handler_check_max_num_cmd_args(command_t *p_cmd)
{
    command_handler_entry_t *p_entry;
    p_entry = __app_cmd_handler_find_cmd_handler_entry(p_cmd);

    if (p_entry && p_entry->args_list) {
        return strlen(p_entry->args_list);
    }
    else {
        return 0;
    }
}

uint32_t app_cmd_handler_process(command_t *p_cmd)
{
    uint32_t res;
    command_handler_entry_t *p_cmd_entry;

    p_cmd_entry = __app_cmd_handler_find_cmd_handler_entry(p_cmd);
    if (!p_cmd_entry) {
        res = FAILURE;
        goto exit;
    }

    if (p_cmd_entry->preproc) {
        res = p_cmd_entry->preproc(p_cmd);

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

uint32_t app_cmd_handler_check_exited(command_t *p_cmd)
{
    if (!strcmp(p_cmd->cmd_name, "app") &&
        !strcmp(p_cmd->subcmd_name, "exit")) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void app_cmd_handler_show_usage(command_t *p_cmd)
{
    command_handler_entry_t *p_cmd_entry;

    p_cmd_entry = __app_cmd_handler_find_cmd_handler_entry(p_cmd);

    if (!p_cmd_entry || !p_cmd_entry->usage) {
        return;
    }

    p_cmd_entry->usage(p_cmd);

    return;
}
