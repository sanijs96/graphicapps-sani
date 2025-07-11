#include <stdio.h>
#include <malloc.h>

#include "common/common_def.h"
#include "vulkan/queue_types.h"

#include "vulkan_device_queue.h"

typedef struct queue_ctx {
    uint32_t *p_queue_states;
    float *p_queue_priorities;
    VkQueue *p_queues;
} queue_ctx_t;

typedef struct __queue_family_ctx {
    uint32_t state;
    uint32_t family_idx;
    uint32_t queue_count;
    queue_ctx_t queues_ctx;
} queue_family_ctx_t;

static struct {
    VkPhysicalDevice *p_phydev;
    queue_family_ctx_t queue_families[NUM_VULKAN_DEVICE_QUEUE_TYPES];
} queue_sets_ctx;

void device_queue_init_ctx(void)
{
    queue_sets_ctx.p_phydev = NULL;

    for (uint32_t idx = 0; idx < NUM_VULKAN_DEVICE_QUEUE_TYPES; idx++) {
        queue_sets_ctx.queue_families[idx].state = VULKAN_DEVICE_QUEUE_FAMILY_STATE_DISABLED;
        queue_sets_ctx.queue_families[idx].family_idx = 0;
        queue_sets_ctx.queue_families[idx].queue_count = 0;
        queue_sets_ctx.queue_families[idx].queues_ctx.p_queue_priorities = NULL;
        queue_sets_ctx.queue_families[idx].queues_ctx.p_queues = NULL;
        queue_sets_ctx.queue_families[idx].queues_ctx.p_queue_states = NULL;
    }
}

static void __device_queue_setup_default_queue_create_info(VkDeviceQueueCreateInfo *p_queue_info)
{
    p_queue_info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    p_queue_info->pNext = NULL;
    p_queue_info->flags = 0;
}

uint32_t device_queue_setup_queue_create_info(VkDeviceQueueCreateInfo *p_queues_info)
{
    uint32_t queue_info_count;
    queue_family_ctx_t *p_families_ctx;

    p_families_ctx = queue_sets_ctx.queue_families;

    queue_info_count = device_queue_get_total_queue_family_count();

    for (uint32_t idx = 0; idx < queue_info_count; idx++) {
        __device_queue_setup_default_queue_create_info(&p_queues_info[idx]);

        if (p_families_ctx[idx].state == VULKAN_DEVICE_QUEUE_FAMILY_STATE_ENABLED) {
            p_queues_info[idx].queueCount = p_families_ctx[idx].queue_count;
            p_queues_info[idx].queueFamilyIndex = p_families_ctx[idx].family_idx;
            p_queues_info[idx].pQueuePriorities = p_families_ctx[idx].queues_ctx.p_queue_priorities;
        }
    }

    return SUCCESS;
}

uint32_t device_queue_get_total_queue_family_count(void)
{
    uint32_t queue_type;
    uint32_t families_idx[NUM_VULKAN_DEVICE_QUEUE_TYPES];
    uint32_t queue_info_count;

    queue_info_count = 0;

    for (uint32_t idx = 0; idx < NUM_VULKAN_DEVICE_QUEUE_TYPES; idx++) {
        families_idx[idx] = device_queue_get_queue_family_idx(idx);
    }

    queue_type = VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS;
    queue_info_count++;

    queue_type = VULKAN_DEVICE_QUEUE_TYPE_COMPUTE;
    if (families_idx[queue_type] != families_idx[VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS]) {
        queue_info_count++;
    }

    queue_type = VULKAN_DEVICE_QUEUE_TYPE_COMPUTE;
    if ((families_idx[queue_type] != families_idx[VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS]) &&
            (families_idx[queue_type] != families_idx[VULKAN_DEVICE_QUEUE_TYPE_COMPUTE])) {
        queue_info_count++;
    }

    return queue_info_count;
}

uint32_t device_queue_get_queue_family_idx(uint32_t type)
{
    return queue_sets_ctx.queue_families[type].family_idx;
}

uint32_t device_queue_get_queue_family_state(uint32_t type)
{
    return queue_sets_ctx.queue_families[type].state;
}

static uint32_t __device_queue_setup_queue_family_ctx(uint32_t type, VkPhysicalDevice *p_phydev)
{
    uint32_t queue_flag;
    uint32_t queue_family_count;
    queue_ctx_t *p_queues_ctx;
    queue_family_ctx_t *p_family_ctx;

    p_family_ctx = &queue_sets_ctx.queue_families[type];

    p_queues_ctx = &p_family_ctx->queues_ctx;

    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_family_count, NULL);

    VkQueueFamilyProperties families[queue_family_count];

    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_family_count, &families[0]);

    if (type == VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS) {
        queue_flag = VK_QUEUE_GRAPHICS_BIT;
    }
    else if (type == VULKAN_DEVICE_QUEUE_TYPE_COMPUTE) {
        queue_flag = VK_QUEUE_COMPUTE_BIT;
    }
    else if (type == VULKAN_DEVICE_QUEUE_TYPE_TRANSFER) {
        queue_flag = VK_QUEUE_TRANSFER_BIT;
    }
    else {
        printf("queue family type %u not supported\n", type);
        return FAILURE;
    }

    for (uint32_t family_idx = 0; family_idx < queue_family_count; family_idx++) {
        uint32_t queue_count;

        if (!(families[family_idx].queueFlags & queue_flag)) {
            continue;
        }

        p_family_ctx->family_idx = family_idx;
        p_family_ctx->queue_count = families[family_idx].queueCount;

        queue_count = p_family_ctx->queue_count;

        p_queues_ctx->p_queues = (VkQueue *)malloc(sizeof(VkQueue) * queue_count);
        p_queues_ctx->p_queue_states = (uint32_t *)malloc(sizeof(uint32_t) * queue_count);
        p_queues_ctx->p_queue_priorities = (float *)malloc(sizeof(float) * queue_count);

        for (uint32_t queue_idx = 0; queue_idx < queue_count; queue_idx++) {
            p_queues_ctx->p_queue_priorities[queue_idx] = 1.0f;
            p_queues_ctx->p_queue_states[queue_idx] = VULKAN_DEVICE_QUEUE_STATE_UNUSED;
        }

        return SUCCESS;
    }

    return FAILURE;
}

uint32_t device_queue_enable_queue_family(uint32_t type, VkPhysicalDevice *p_phydev)
{
    uint32_t res;
    queue_family_ctx_t *p_family_ctx;

    if (type >= NUM_VULKAN_DEVICE_QUEUE_TYPES) {
        return FAILURE;
    }

    p_family_ctx = &queue_sets_ctx.queue_families[type];

    if (queue_sets_ctx.p_phydev == NULL) {
        queue_sets_ctx.p_phydev = p_phydev;
    }
    else if (queue_sets_ctx.p_phydev != p_phydev) {
        printf("another physical device is already registered\n");
        return FAILURE;
    }

    res = __device_queue_setup_queue_family_ctx(type, p_phydev);

    p_family_ctx->state = VULKAN_DEVICE_QUEUE_FAMILY_STATE_ENABLED;

    return res;
}

uint32_t device_queue_get_queue_count(uint32_t type)
{
    return queue_sets_ctx.queue_families[type].queue_count;
}

uint32_t device_queue_get_queue_state(uint32_t type, uint32_t idx)
{
    return queue_sets_ctx.queue_families[type].queues_ctx.p_queue_states[idx];
}

VkQueue *device_queue_get_queue_object(uint32_t type, uint32_t idx)
{
    return &queue_sets_ctx.queue_families[type].queues_ctx.p_queues[idx];
}

static void __device_queue_add_queue_ctx(uint32_t type, VkDevice *p_device)
{
    queue_family_ctx_t *p_family_ctx;

    p_family_ctx = &queue_sets_ctx.queue_families[type];

    for (uint32_t queue_idx = 0; queue_idx < p_family_ctx->queue_count; queue_idx++) {
        vkGetDeviceQueue(*p_device, p_family_ctx->family_idx, queue_idx,
                            &p_family_ctx->queues_ctx.p_queues[queue_idx]);
    }
}

uint32_t device_queue_enable_queues(uint32_t type, VkDevice *p_device)
{
    if (type >= NUM_VULKAN_DEVICE_QUEUE_TYPES) {
        printf("invalid queue type\n");
        return FAILURE;
    }

    __device_queue_add_queue_ctx(type, p_device);

    return SUCCESS;
}

uint32_t device_queue_lock(uint32_t type, uint32_t idx)
{
    queue_ctx_t *p_queues_ctx;

    p_queues_ctx = &queue_sets_ctx.queue_families[type].queues_ctx;

    // TODO: synchronize using thread management
    if (device_queue_get_queue_state(type, idx) == VULKAN_DEVICE_QUEUE_STATE_USED) {
        return FAILURE;
    }

    p_queues_ctx->p_queue_states[idx] = VULKAN_DEVICE_QUEUE_STATE_USED;

    if (device_queue_get_queue_state(type, idx) == VULKAN_DEVICE_QUEUE_STATE_UNUSED) {
        return FAILURE;
    }

    return SUCCESS;
}

void device_queue_unlock(uint32_t type, uint32_t idx)
{
    queue_ctx_t *p_queues_ctx;

    p_queues_ctx = &queue_sets_ctx.queue_families[type].queues_ctx;

    p_queues_ctx->p_queue_states[idx] = VULKAN_DEVICE_QUEUE_STATE_UNUSED;
}
