#ifndef __APPS_CMD_DEF_H__
#define __APPS_CMD_DEF_H__

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define APPS_CMD_TYPE_SHIFT             (16)
#define APPS_CMD_ID_SHIFT               (8)

#define MAX_LENGTH_APP_CMD              (VK_MAX_EXTENSION_NAME_SIZE + 100)

enum apps_param_type_vulkan {
    APPS_PARAM_TYPE_VULKAN_LAYER_NAME = 'n',

    APPS_PARAM_TYPE_VULKAN_EXTENSION_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_EXTENSION_SCOPE = 's',
    APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX = 'i',

    APPS_PARAM_TYPE_VULKAN_DEVICE_PHYDEV_IDX = 'i',
    APPS_PARAM_TYPE_VULKAN_DEVICE_SHOW_EXTENSION = 'e',

    APPS_PARAM_TYPE_VULKAN_PIPELINE_BINDING_CMDBUF_IDX = 'b',
    APPS_PARAM_TYPE_VULKAN_PIPELINE_SHADER_FILENAME = 'f',
    APPS_PARAM_TYPE_VULKAN_PIPELINE_STAGE_IDX = 'i',

    APPS_PARAM_TYPE_VULKAN_CMDBUF_COMMAND_TYPE = 'c',
    APPS_PARAM_TYPE_VULKAN_CMDBUF_BUFFER_IDX = 'i',
};

enum apps_param_type_etc {
    APPS_PARAM_TYPE_ETC_WINDOW_WIDTH = 'w',
    APPS_PARAM_TYPE_ETC_WINDOW_HEIGHT = 'h',

    APPS_PARAM_TYPE_ETC_WINDOW_INSTANCE = 'i',
};

enum apps_param_type_app {
    APPS_PARAM_TYPE_APP_SCRIPTFILE_NAME = 'f',
};

#define PARAM_VK(type)    (APPS_PARAM_TYPE_VULKAN_ ## type)
#define PARAM_ETC(type)    (APPS_PARAM_TYPE_ETC_ ## type)
#define PARAM_APP(type)    (APPS_PARAM_TYPE_APP_ ## type)

typedef struct __command_arg {
    char type;
    char *value;
} command_arg_t;

typedef struct __command {
    char *cmd_name;
    char *subcmd_name;

    uint32_t num_args;
    command_arg_t *p_args;
} command_t;

#endif
