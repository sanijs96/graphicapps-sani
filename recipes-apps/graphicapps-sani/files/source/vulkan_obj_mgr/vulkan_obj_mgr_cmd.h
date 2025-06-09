#ifndef __VULKAN_OBJ_MGR_CMD_LIST_H__
#define __VULKAN_OBJ_MGR_CMD_LIST_H__

#include "app/app_cmd_def.h"

#include "vulkan_obj_mgr.h"

enum apps_cmd_id_vulkan {
    APPS_CMD_ID_VULKAN_INSTANCE,
    APPS_CMD_ID_VULKAN_LAYER,
    APPS_CMD_ID_VULKAN_EXTENSION,
    APPS_CMD_ID_VULKAN_DEVICE,
    APPS_CMD_ID_VULKAN_PIPELINE,
};

enum apps_subcmd_id_vulkan {
    APPS_SUBCMD_ID_VULKAN_INSTANCE_CREATE,

    APPS_SUBCMD_ID_VULKAN_LAYER_ADD,
    APPS_SUBCMD_ID_VULKAN_LAYER_DEL,
    APPS_SUBCMD_ID_VULKAN_LAYER_SHOW_LIST,

    APPS_SUBCMD_ID_VULKAN_EXTENSION_ADD,
    APPS_SUBCMD_ID_VULKAN_EXTENSION_DEL,
    APPS_SUBCMD_ID_VULKAN_EXTENSION_SHOW_LIST,

    APPS_SUBCMD_ID_VULKAN_DEVICE_CREATE,
    APPS_SUBCMD_ID_VULKAN_DEVICE_SHOW_LIST,

    APPS_SUBCMD_ID_VULKAN_PIPELINE_ADD_CMD,
    APPS_SUBCMD_ID_VULKAN_PIPELINE_BIND_SHADER,
    APPS_SUBCMD_ID_VULKAN_PIPELINE_BIND_OBJECT,
};

enum apps_param_type_vulkan {
    APPS_PARAM_TYPE_VULKAN_LAYER_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_LAYER_SCOPE = 's',

    APPS_PARAM_TYPE_VULKAN_EXTENSION_NAME = 'n',
    APPS_PARAM_TYPE_VULKAN_EXTENSION_SCOPE = 's',

    APPS_PARAM_TYPE_VULKAN_DEVICE_PHYDEV_IDX = 'd',
};

uint32_t _vulkan_obj_mgr_cmd_check_arguments(command_arg_t *p_arg);

uint32_t _vulkan_obj_mgr_enable_layer(command_t *p_cmd);
uint32_t _vulkan_obj_mgr_disable_layer(command_t *p_cmd);

uint32_t _vulkan_obj_mgr_enable_extension(command_t *p_cmd);
uint32_t _vulkan_obj_mgr_disable_extension(command_t *p_cmd);

uint32_t _vulkan_obj_mgr_create_instance(command_t *p_cmd);;
uint32_t _vulkan_obj_mgr_create_device(command_t *p_cmd);;

#endif
