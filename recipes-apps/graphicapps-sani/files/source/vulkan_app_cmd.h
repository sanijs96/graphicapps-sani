#ifndef __VULKAN_APP_CMD_H__
#define __VULKAN_APP_CMD_H__

#include "app/app_cmd_def.h"

uint32_t vulkan_app_cmd_enable_layer(command_t *p_cmd);
uint32_t vulkan_app_cmd_disable_layer(command_t *p_cmd);
uint32_t vulkan_app_cmd_show_layers_list(command_t *p_cmd);

uint32_t vulkan_app_cmd_enable_extension(command_t *p_cmd);
uint32_t vulkan_app_cmd_disable_extension(command_t *p_cmd);
uint32_t vulkan_app_cmd_show_extensions_list(command_t *p_cmd);

uint32_t vulkan_app_cmd_create_instance(command_t *p_cmd);
uint32_t vulkan_app_cmd_delete_instance(command_t *p_cmd);

uint32_t vulkan_app_cmd_create_device(command_t *p_cmd);
uint32_t vulkan_app_cmd_show_devices_list(command_t *p_cmd);

uint32_t vulkan_app_cmd_init_pipeline_ctx(command_t *p_cmd);
uint32_t vulkan_app_cmd_add_pipeline_stage(command_t *p_cmd);
uint32_t vulkan_app_cmd_create_pipeline(command_t *p_cmd);
uint32_t vulkan_app_cmd_show_pipeline_info(command_t *p_cmd);

uint32_t vulkan_app_cmd_allocate_command_buffer(command_t *p_cmd);
uint32_t vulkan_app_cmd_add_vulkan_command(command_t *p_cmd);
uint32_t vulkan_app_cmd_show_command_buffer_info(command_t *p_cmd);
uint32_t vulkan_app_cmd_run_commands(command_t *p_cmd);

uint32_t vulkan_app_cmd_create_resource(command_t *p_cmd);

#endif
