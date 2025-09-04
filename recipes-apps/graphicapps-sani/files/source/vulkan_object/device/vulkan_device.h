#ifndef __VULKAN_DEVICE_H__
#define __VULKAN_DEVICE_H__

#include <vulkan/vulkan_core.h>

#define MAX_VULKAN_DEVICE_WAIT_SEMPAHORES_COUNT     (1)
#define MAX_VULKAN_DEVICE_SIGNAL_SEMPAHORES_COUNT   (1)
#define MAX_VULKAN_DEVICE_CMD_BUFFERS_COUNT         (1)

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
uint32_t device_get_queue_family_idx(uint32_t queue_type);

uint32_t device_add_cmdbuf_ctx(VkCommandBuffer *p_cmdbuf);
uint32_t device_add_wait_semaphore_ctx(VkSemaphore *p_semaphore, VkPipelineStageFlags stage);
uint32_t device_add_signal_semaphore_ctx(VkSemaphore *p_semaphore);
uint32_t device_get_wait_semaphore_count(void);
uint32_t device_get_signal_semaphore_count(void);

VkPipelineStageFlags *device_get_wait_semaphore_stages(void);
VkSemaphore *device_get_wait_semaphore_objects(void);
VkSemaphore *device_get_signal_semaphore_objects(void);

uint32_t device_get_available_queue_idx(uint32_t type);
VkQueue *device_get_queue_object(uint32_t type, uint32_t idx);

uint32_t device_lock_queue(uint32_t type, uint32_t queue_idx);
void device_unlock_queue(uint32_t type, uint32_t queue_idx);
uint32_t device_submit_queue(VkQueue *p_queue, VkSubmitInfo *p_submit_info, VkFence *p_fence);

void device_get_device_queue_properties(uint32_t phydev_idx, VkQueueFamilyProperties *properteis);

#endif
