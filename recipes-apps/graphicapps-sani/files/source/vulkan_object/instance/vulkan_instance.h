#ifndef __VULKAN_INSTANCE_H__
#define __VULKAN_INSTANCE_H__

#include <vulkan/vulkan_core.h>

enum instance_creation_state {
    VULKAN_INSTANCE_CREATION_STATE_DEFAULT = 0,
    VULKAN_INSTANCE_CREATION_STATE_CREATED,
    VULKAN_INSTANCE_CREATION_STATE_DELETED,
};

void instance_init(void);

uint32_t instance_check_creation_state(void);

void instance_add_layer_info(uint32_t layers_count, char **pp_layers_name);
void instance_add_extension_info(uint32_t extension_count, char **pp_extensions_name);

uint32_t instance_create(void);

VkInstance *instance_get_instance_object(void);

void instance_destroy(void);

#endif
