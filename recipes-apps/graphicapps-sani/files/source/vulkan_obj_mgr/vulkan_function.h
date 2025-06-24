#ifndef __VULKAN_FUNCTION_H__
#define __VULKAN_FUNCTION_H__

#include <vulkan/vulkan_core.h>

enum function_state {
    VULKAN_FUNCTION_STATE_DEFAULT = 0,
    VULKAN_FUNCTION_STATE_DISABLED,
    VULKAN_FUNCTION_STATE_ENABLED,
    VULKAN_FUNCTION_STATE_INVALID,
};

enum function_scope {
    VULKAN_FUNCTION_SCOPE_DEFAULT = 0,
    VULKAN_FUNCTION_SCOPE_DEVICE,
    VULKAN_FUNCTION_SCOPE_INSTANCE,
    VULKAN_FUNCTION_SCOPE_INVALID,
};

void function_init(void);
uint32_t function_init_phydev_extension_info(void);

uint32_t function_enable_layer(char *layer_name);
uint32_t function_disable_layer(char *layer_name);
uint32_t function_check_layer_state(char *layer_name);
void function_get_layers_name_list(uint32_t state, char ** names_list);

uint32_t function_enable_extension(char *extension_name);
uint32_t function_disable_extension(char *extension_name);
uint32_t function_check_extension_state(char *extension_name);
void function_get_extensions_name_list(uint32_t state, char ** names_list);

uint32_t function_enable_phydev_extension(char *extension_name, uint32_t phydev_idx);
uint32_t function_disable_phydev_extension(char *extension_name, uint32_t phydev_idx);
void function_get_phydev_exts_name_list(uint32_t state, char ** names_list, uint32_t phydev_idx);

uint32_t function_get_layers_count(uint32_t state);
uint32_t function_get_extensions_count(uint32_t state);
uint32_t function_get_phydev_exts_count(uint32_t state, uint32_t phydev_idx);

#endif
