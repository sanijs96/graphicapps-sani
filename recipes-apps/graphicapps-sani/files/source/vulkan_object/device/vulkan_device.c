#include <malloc.h>
#include <string.h>

#include "common/common_def.h"
#include "vulkan/queue_types.h"

#include "vulkan_device.h"
#include "queue/vulkan_device_queue.h"

typedef struct function_ctx {
    uint32_t count;
    char **names_list;
} function_ctx_t;

typedef struct semaphore_ctx {
    uint32_t wait_count;
    VkPipelineStageFlags wait_stages[MAX_VULKAN_DEVICE_WAIT_SEMPAHORES_COUNT];
    VkSemaphore wait_semaphores[MAX_VULKAN_DEVICE_WAIT_SEMPAHORES_COUNT];

    uint32_t signal_count;
    VkSemaphore signal_semaphores[MAX_VULKAN_DEVICE_SIGNAL_SEMPAHORES_COUNT];
} semaphore_ctx_t;

typedef struct cmdbuf_ctx {
    uint32_t count;
    VkCommandBuffer buffers[MAX_VULKAN_DEVICE_CMD_BUFFERS_COUNT];
} cmdbuf_ctx_t;

struct device_ctx_t {
    uint32_t status;
    uint32_t pdev_count;
    VkPhysicalDevice *p_phydevs;

    VkDevice device;
    uint32_t phydev_idx;
    function_ctx_t layer_ctx;
    function_ctx_t extension_ctx;

    cmdbuf_ctx_t cmd_buffers_ctx;
    semaphore_ctx_t semaphores_ctx;
} device_ctx;

void device_init_device_ctx(VkInstance *p_instance)
{
    uint32_t phydev_count;

    vkEnumeratePhysicalDevices(*p_instance, &phydev_count, NULL);
    device_ctx.pdev_count = phydev_count;

    device_ctx.p_phydevs = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * phydev_count);
    vkEnumeratePhysicalDevices(*p_instance, &phydev_count, device_ctx.p_phydevs);

    device_ctx.phydev_idx = 0;
    device_ctx.status = VULKAN_DEVICE_CREATION_STATE_DEFAULT;

    device_ctx.cmd_buffers_ctx.count = 0;
    device_ctx.semaphores_ctx.wait_count = 0;
    device_ctx.semaphores_ctx.signal_count = 0;

    device_queue_init_ctx();
}

uint32_t device_get_phydevs_count(void)
{
    return device_ctx.pdev_count;
}

void device_add_enabled_layers_list(uint32_t layer_count, char **layer_name_list)
{
    device_ctx.layer_ctx.count = layer_count;
    device_ctx.layer_ctx.names_list = layer_name_list;
}

void device_add_enabled_extensions_list(uint32_t ext_count, char **extension_name_list)
{
    device_ctx.extension_ctx.count = ext_count;
    device_ctx.extension_ctx.names_list = extension_name_list;
}

static uint32_t __device_enable_queue_family(uint32_t type, VkPhysicalDevice *p_phydev)
{
    if (device_queue_enable_queue_family(type, p_phydev) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __device_enable_queues(uint32_t type)
{
    if (device_queue_enable_queues(type, &device_ctx.device) == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t device_create(uint32_t phydev_idx)
{
    uint32_t res;
    uint32_t queue_family_idx;
    uint32_t queue_info_count;
    VkPhysicalDevice *p_phydev;
    VkDeviceCreateInfo device_info;

    if (device_get_current_status() == VULKAN_DEVICE_CREATION_STATE_CREATED) {
        printf("device already created\n");
        return FAILURE;
    }

    p_phydev = &device_ctx.p_phydevs[phydev_idx];
    for (uint32_t type = 0; type < NUM_VULKAN_DEVICE_QUEUE_TYPES; type++) {
        if (__device_enable_queue_family(type, p_phydev) == FAILURE) {
            printf("queue type %u enable failure\n", type);
        }
    }

    queue_info_count = device_queue_get_total_queue_family_count();

    VkDeviceQueueCreateInfo queues_info[queue_info_count];

    res = device_queue_setup_queue_create_info(queues_info);
    if (res == FAILURE) {
        printf("queue create info setup failure\n");
        return FAILURE;
    }

    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.flags = 0;

    device_info.pQueueCreateInfos = queues_info;
    device_info.queueCreateInfoCount = queue_info_count;

    device_info.enabledLayerCount = device_ctx.layer_ctx.count;
    device_info.ppEnabledLayerNames = (const char * const *)device_ctx.layer_ctx.names_list;

    device_info.enabledExtensionCount = device_ctx.extension_ctx.count;
    device_info.ppEnabledExtensionNames = (const char * const *)device_ctx.extension_ctx.names_list;

    device_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(*p_phydev, &device_info, NULL, &device_ctx.device);
    if (res != VK_SUCCESS) {
        printf("device creation failure: %d\n", res);
        return FAILURE;
    }

    for (uint32_t idx = 0; idx < NUM_VULKAN_DEVICE_QUEUE_TYPES; idx++) {
        if (__device_enable_queues(idx) == FAILURE) {
            printf("enable queue type %u failure\n", idx);
            return FAILURE;
        }
    }

    device_ctx.phydev_idx = phydev_idx;
    device_ctx.status = VULKAN_DEVICE_CREATION_STATE_CREATED;

    return SUCCESS;
}

uint32_t device_get_current_status(void)
{
    return device_ctx.status;
}

uint32_t device_get_current_phydev_idx(void)
{
    return device_ctx.phydev_idx;
}

void device_get_device_property(uint32_t phydev_idx, VkPhysicalDeviceProperties *p_property)
{
    VkPhysicalDevice *p_phydev;

    p_phydev = &device_ctx.p_phydevs[phydev_idx];

    vkGetPhysicalDeviceProperties(*p_phydev, p_property);
}

uint32_t device_get_device_queue_property_count(uint32_t phydev_idx)
{
    uint32_t property_count;
    VkPhysicalDevice *p_phydev;

    p_phydev = &device_ctx.p_phydevs[phydev_idx];

    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &property_count, NULL);

    return property_count;
}

uint32_t device_get_queue_family_idx(uint32_t queue_type)
{
    return device_queue_get_queue_family_idx(queue_type);
}

VkDevice *device_get_current_device_object(void)
{
    if (device_get_current_status() != VULKAN_DEVICE_CREATION_STATE_CREATED) {
        printf("device is not created\n");

        return NULL;
    }

    return &device_ctx.device;
}

VkPhysicalDevice *device_get_phydev_object(uint32_t phydev_idx)
{
    return &device_ctx.p_phydevs[phydev_idx];
}

void device_get_device_queue_properties(uint32_t phydev_idx, VkQueueFamilyProperties *p_properties)
{
    uint32_t property_count;
    VkPhysicalDevice *p_phydev;

    p_phydev = &device_ctx.p_phydevs[phydev_idx];

    property_count = device_get_device_queue_property_count(phydev_idx);

    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &property_count, p_properties);
}

uint32_t device_add_wait_semaphore_ctx(VkSemaphore *p_semaphore, VkPipelineStageFlags stage)
{
    uint32_t semaphore_entry_idx;
    semaphore_ctx_t *p_semaphore_ctx;

    p_semaphore_ctx = &device_ctx.semaphores_ctx;

    semaphore_entry_idx = p_semaphore_ctx->wait_count;
    if (semaphore_entry_idx == MAX_VULKAN_DEVICE_WAIT_SEMPAHORES_COUNT) {
        printf("semaphore is fully registered\n");
        return FAILURE;
    }

    p_semaphore_ctx->wait_stages[semaphore_entry_idx] = stage;
    p_semaphore_ctx->wait_semaphores[semaphore_entry_idx] = *p_semaphore;

    p_semaphore_ctx->wait_count++;

    return SUCCESS;
}

uint32_t device_add_signal_semaphore_ctx(VkSemaphore *p_semaphore)
{
    uint32_t semaphore_entry_idx;
    semaphore_ctx_t *p_semaphore_ctx;

    p_semaphore_ctx = &device_ctx.semaphores_ctx;
    semaphore_entry_idx = p_semaphore_ctx->signal_count;
    if (semaphore_entry_idx == MAX_VULKAN_DEVICE_SIGNAL_SEMPAHORES_COUNT) {
        printf("semaphore is fully registered\n");
        return FAILURE;
    }

    p_semaphore_ctx->signal_semaphores[semaphore_entry_idx] = *p_semaphore;

    p_semaphore_ctx->signal_count++;

    return SUCCESS;
}

uint32_t device_get_wait_semaphore_count(void)
{
    return device_ctx.semaphores_ctx.wait_count;
}

uint32_t device_get_signal_semaphore_count(void)
{
    return device_ctx.semaphores_ctx.signal_count;
}

VkPipelineStageFlags *device_get_wait_semaphore_stages(void)
{
    return &device_ctx.semaphores_ctx.wait_stages[0];
}

VkSemaphore *device_get_wait_semaphore_objects(void)
{
    return &device_ctx.semaphores_ctx.wait_semaphores[0];
}

VkSemaphore *device_get_signal_semaphore_objects(void)
{
    return &device_ctx.semaphores_ctx.signal_semaphores[0];
}


uint32_t device_add_cmdbuf_ctx(VkCommandBuffer *p_cmdbuf)
{
    uint32_t cmdbuf_entry_idx;
    cmdbuf_ctx_t *p_cmdbuf_ctx;

    p_cmdbuf_ctx = &device_ctx.cmd_buffers_ctx;

    cmdbuf_entry_idx = p_cmdbuf_ctx->count;

    if (cmdbuf_entry_idx == MAX_VULKAN_DEVICE_CMD_BUFFERS_COUNT) {
        printf("command buffer is fully registered\n");
        return FAILURE;
    }

    p_cmdbuf_ctx->buffers[cmdbuf_entry_idx] = *p_cmdbuf;

    p_cmdbuf_ctx->count++;

    return SUCCESS;
}

static uint32_t __device_create_fence(VkFence *p_fence)
{
    uint32_t res;
    VkFenceCreateInfo fence_info;

    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.pNext = NULL;
    fence_info.flags = 0;

    res = vkCreateFence(device_ctx.device, &fence_info, NULL, p_fence);
    if (res != VK_SUCCESS) {
        printf("fence creation failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t device_lock_queue(uint32_t type, uint32_t queue_idx)
{
    return device_queue_lock(type, queue_idx);
}

void device_unlock_queue(uint32_t type, uint32_t queue_idx)
{
    device_queue_unlock(type, queue_idx);
}

uint32_t device_get_available_queue_idx(uint32_t type)
{
    uint32_t queue_count;

    queue_count = device_queue_get_queue_count(type);

    for (uint32_t idx = 0; idx < queue_count; idx++) {
        if (device_queue_get_queue_state(type, idx) == VULKAN_DEVICE_QUEUE_STATE_UNUSED) {
            return idx;
        }
    }

    return queue_count;
}

VkQueue *device_get_queue_object(uint32_t type, uint32_t idx)
{
    return device_queue_get_queue_object(type, idx);
}

uint32_t device_submit_queue(VkQueue *p_queue, VkSubmitInfo *p_submit_info, VkFence *p_fence)
{
    uint32_t res;

    res = vkQueueSubmit(*p_queue, 1, p_submit_info, *p_fence);
    if (res != VK_SUCCESS) {
        printf("queue submission failure: %d\n", res);

        return FAILURE;
    }

    return SUCCESS;
}
