#ifndef __APPS_CMD_DEF_H__
#define __APPS_CMD_DEF_H__

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#include "common/common_def.h"

#define APPS_CMD_TYPE_SHIFT                 (16)
#define APPS_CMD_ID_SHIFT                   (8)

#define MAX_LENGTH_APP_CMD                  (VK_MAX_EXTENSION_NAME_SIZE + 100)

enum apps_param_type_vulkan {
    APPS_PARAM_TYPE_VULKAN_LAYER_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_LAYER_SCOPE = 's',

    APPS_PARAM_TYPE_VULKAN_EXTENSION_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_EXTENSION_SCOPE = 's',
    APPS_PARAM_TYPE_VULKAN_EXTENSION_PHYDEV_IDX = 'i',

    APPS_PARAM_TYPE_VULKAN_DEVICE_PHYDEV_IDX = 'i',
    APPS_PARAM_TYPE_VULKAN_DEVICE_SHOW_EXTENSION = 'e',

    APPS_PARAM_TYPE_VULKAN_PIPELINE_SHADER_FILENAME = 'f',
    APPS_PARAM_TYPE_VULKAN_PIPELINE_RESOURCE_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_PIPELINE_STAGE_TYPE = 's',

    APPS_PARAM_TYPE_VULKAN_CMDBUF_BUFFER_TYPE = 'b',
    APPS_PARAM_TYPE_VULKAN_CMDBUF_COMMAND_TYPE = 'c',
    APPS_PARAM_TYPE_VULKAN_CMDBUF_BUFFER_IDX = 'i',
    APPS_PARAM_TYPE_VULKAN_CMDBUF_RESOURCE_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_CMDBUF_PIPELINE_IDX = 'p',

    APPS_PARAM_TYPE_VULKAN_RESOURCE_OBJECT_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_RESOURCE_PIPELINE_IDX = 'p',
};

enum apps_param_type_etc {
    APPS_PARAM_TYPE_ETC_WINDOW_WIDTH = 'w',
    APPS_PARAM_TYPE_ETC_WINDOW_HEIGHT = 'h',

    APPS_PARAM_TYPE_ETC_WINDOW_INSTANCE = 'i',
};

enum apps_param_type_app {
    APPS_PARAM_TYPE_APP_SCRIPTFILE_NAME = 'f',
    APPS_PARAM_TYPE_APP_DATAFILE_NAME = 'f',
    APPS_PARAM_TYPE_APP_DATASCRIPT_NAME = APPS_PARAM_TYPE_APP_SCRIPTFILE_NAME,
};

#define PARAM_VK(type)    (APPS_PARAM_TYPE_VULKAN_ ## type)
#define PARAM_ETC(type)    (APPS_PARAM_TYPE_ETC_ ## type)
#define PARAM_APP(type)    (APPS_PARAM_TYPE_APP_ ## type)

typedef struct __command_arg {
    char type;
    char value[MAX_LENGTH_ARGUMENT_NAME];
} command_arg_t;

typedef struct __command {
    uint32_t retval;

    char cmd_name[MAX_LENGTH_ARGUMENT_NAME];
    char subcmd_name[MAX_LENGTH_ARGUMENT_NAME];
    char saved_argname[MAX_LENGTH_ARGUMENT_NAME];

    uint32_t num_args;
    command_arg_t *p_args;
} command_t;

#endif
