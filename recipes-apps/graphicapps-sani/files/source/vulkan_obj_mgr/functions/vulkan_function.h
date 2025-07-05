#ifndef __VULKAN_FUNCTION_H__
#define __VULKAN_FUNCTION_H__

#include <vulkan/vulkan_core.h>

enum function_state {
    VULKAN_FUNCTION_STATE_DEFAULT = 0,
    VULKAN_FUNCTION_STATE_DISABLED,
    VULKAN_FUNCTION_STATE_ENABLED,
    VULKAN_FUNCTION_STATE_INVALID,
};

void function_init_layers_ctx(void);
uint32_t function_init_phydevs_ctx(uint32_t phydev_count, VkPhysicalDevice *p_phydevs);

uint32_t function_enable_instance_layer(char *layer_name);
uint32_t function_disable_instance_layer(char *layer_name);
uint32_t function_check_instance_layer_state(char *layer_name);
uint32_t function_get_instance_layers_count(uint32_t state);
void function_get_instance_layers_name_list(uint32_t state, char ** names_list);

uint32_t function_enable_instance_extension(char *extension_name);
uint32_t function_disable_instance_extension(char *extension_name);
uint32_t function_check_instance_extension_state(char *extension_name);
uint32_t function_get_instance_extensions_count(uint32_t state);
void function_get_instance_extensions_name_list(uint32_t state, char ** names_list);

uint32_t function_enable_phydev_layer(char *layer_name, uint32_t phydev_idx,
                                                    VkPhysicalDevice *p_phydev);
uint32_t function_disable_phydev_layer(char *layer_name, uint32_t phydev_idx,
                                                    VkPhysicalDevice *p_phydev);
uint32_t function_get_phydev_layers_count(uint32_t state, uint32_t phydev_idx);
void function_get_phydev_layers_name_list(uint32_t state, char **names_list,
                                            uint32_t phydev_idx, VkPhysicalDevice *p_phydev);

uint32_t function_enable_phydev_extension(char *extension_name, uint32_t phydev_idx,
                                                            VkPhysicalDevice *p_phydev);
uint32_t function_disable_phydev_extension(char *extension_name, uint32_t phydev_idx,
                                                            VkPhysicalDevice *p_phydev);
void function_get_phydev_extensions_name_list(uint32_t state, char ** names_list,
                                        uint32_t phydev_idx, VkPhysicalDevice *p_phydev);
uint32_t function_get_phydev_extensions_count(uint32_t state, uint32_t phydev_idx);

#endif
