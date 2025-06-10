#ifndef __VULKAN_OBJ_MGR_CMD_LIST_H__
#define __VULKAN_OBJ_MGR_CMD_LIST_H__

#include "app/app_cmd_def.h"

uint32_t _vulkan_obj_mgr_cmd_check_arguments(command_arg_t *p_arg);

uint32_t _vulkan_obj_mgr_cmd_enable_layer(command_t *p_cmd);
uint32_t _vulkan_obj_mgr_cmd_disable_layer(command_t *p_cmd);

uint32_t _vulkan_obj_mgr_cmd_enable_extension(command_t *p_cmd);
uint32_t _vulkan_obj_mgr_cmd_disable_extension(command_t *p_cmd);

uint32_t _vulkan_obj_mgr_cmd_create_instance(command_t *p_cmd);;
uint32_t _vulkan_obj_mgr_cmd_create_device(command_t *p_cmd);;

#endif
