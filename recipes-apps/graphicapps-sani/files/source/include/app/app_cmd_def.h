#ifndef __APPS_CMD_DEF_H__
#define __APPS_CMD_DEF_H__

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define APPS_CMD_TYPE_SHIFT             (16)
#define APPS_CMD_ID_SHIFT               (8)

#define MAX_LENGTH_APP_CMD              (VK_MAX_EXTENSION_NAME_SIZE + 100)

//#define _APPS_CMD_OPCODE(cmd_type, cmd_id, subcmd_id)                       \
//                        (cmd_type   << APPS_CMD_TYPE_SHIFT |                \
//                         cmd_id     << APPS_CMD_ID_SHIFT   |                \
//                         subcmd_id)
//
//enum apps_cmd_type {
//    APPS_CMD_TYPE_VULKAN,
//    APPS_CMD_TYPE_ETC,
//};
//
//#define CMD_V(cid)              APPS_CMD_ID_VULKAN_ ## cid
//#define CMD_E(cid)              APPS_CMD_ID_ETC_ ## cid
//
//#define SUBCMD_V(cid, subcid)   APPS_SUBCMD_ID_VULKAN_ ## cid ## _ ## subcid
//#define SUBCMD_E(cid, subcid)   APPS_SUBCMD_ID_ETC_ ## cid ## _ ## subcid
//
//#define OPCODE_VK(cmd_id, subcmd_id) _APPS_CMD_OPCODE(APPS_CMD_TYPE_VULKAN, \
//                                    CMD_V(cmd_id), SUBCMD_V(cmd_id, subcmd_id))
//#define OPCODE_ETC(cmd_id, subcmd_id) _APPS_CMD_OPCODE(APPS_CMD_TYPE_ETC,   \
//                                    CMD_E(cmd_id), SUBCMD_E(cmd_id, subcmd_id))
//
//enum apps_cmd_id_vulkan {
//    APPS_CMD_ID_VULKAN_INSTANCE,
//    APPS_CMD_ID_VULKAN_LAYER,
//    APPS_CMD_ID_VULKAN_EXTENSION,
//    APPS_CMD_ID_VULKAN_DEVICE,
//    APPS_CMD_ID_VULKAN_PIPELINE,
//};
//
//enum apps_subcmd_id_vulkan {
//    APPS_SUBCMD_ID_VULKAN_INSTANCE_CREATE,
//
//    APPS_SUBCMD_ID_VULKAN_LAYER_ADD,
//    APPS_SUBCMD_ID_VULKAN_LAYER_DEL,
//    APPS_SUBCMD_ID_VULKAN_LAYER_SHOW_LIST,
//
//    APPS_SUBCMD_ID_VULKAN_EXTENSION_ADD,
//    APPS_SUBCMD_ID_VULKAN_EXTENSION_DEL,
//    APPS_SUBCMD_ID_VULKAN_EXTENSION_SHOW_LIST,
//
//    APPS_SUBCMD_ID_VULKAN_DEVICE_CREATE,
//    APPS_SUBCMD_ID_VULKAN_DEVICE_SHOW_LIST,
//
//    APPS_SUBCMD_ID_VULKAN_PIPELINE_ADD_CMD,
//    APPS_SUBCMD_ID_VULKAN_PIPELINE_BIND_SHADER,
//    APPS_SUBCMD_ID_VULKAN_PIPELINE_BIND_OBJECT,
//};

enum apps_param_type_vulkan {
    APPS_PARAM_TYPE_VULKAN_LAYER_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_LAYER_SCOPE = 's',

    APPS_PARAM_TYPE_VULKAN_EXTENSION_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_EXTENSION_SCOPE = 's',

    APPS_PARAM_TYPE_VULKAN_DEVICE_PHYDEV_IDX = 'd',
};

enum apps_cmd_id_etc {
    APPS_CMD_ID_ETC_WINDOW,
    APPS_CMD_ID_ETC_CONSOLE,
};

enum apps_subcmd_id_etc {
    APPS_SUBCMD_ID_ETC_WINDOW_DISPLAY,
    APPS_SUBCMD_ID_ETC_WINDOW_RESIZE,
    APPS_SUBCMD_ID_ETC_CONSOLE_EXIT,
};

enum apps_param_type_etc {
    APPS_PARAM_TYPE_ETC_WINDOW_DISPLAY_WIDTH = 'w',
    APPS_PARAM_TYPE_ETC_WINDOW_DISPLAY_HEIGHT = 'h',

    APPS_PARAM_TYPE_ETC_WINDOW_RESIZE_WIDTH = 'w',
    APPS_PARAM_TYPE_ETC_WINDOW_RESIZE_HEIGHT = 'h',
};

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
