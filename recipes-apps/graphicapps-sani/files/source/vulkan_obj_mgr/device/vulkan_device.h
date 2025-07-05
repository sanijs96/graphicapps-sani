#ifndef __VULKAN_DEVICE_H__
#define __VULKAN_DEVICE_H__

#include <vulkan/vulkan_core.h>

enum device_creation_state {
    VULKAN_DEVICE_CREATION_STATE_DEFAULT = 0,
    VULKAN_DEVICE_CREATION_STATE_CREATED,
    VULKAN_DEVICE_CREATION_STATE_DELETED,
};

typedef struct __device_queue_ctx {
    uint32_t *priorities;
    uint32_t num_queues;
} device_queue_ctx_t;

void device_init_device_ctx(VkInstance *p_instance);

void device_add_enabled_layers_list(uint32_t layer_count, char **layer_name_list);
void device_add_enabled_extensions_list(uint32_t ext_count, char **extension_name_list);

uint32_t device_create(uint32_t phydev_idx);

uint32_t device_get_phydevs_count(void);

uint32_t device_get_current_status(void);
uint32_t device_get_current_phydev_idx(void);
VkDevice *device_get_current_device_object(void);
VkPhysicalDevice *device_get_phydev_object(uint32_t phydev_idx);

void device_get_device_property(uint32_t phydev_idx, VkPhysicalDeviceProperties *property);

uint32_t device_get_device_queue_property_count(uint32_t phydev_idx);
uint32_t device_get_queue_idx(uint32_t queue_type);

void device_get_device_queue_properties(uint32_t phydev_idx, VkQueueFamilyProperties *properteis);

#endif
