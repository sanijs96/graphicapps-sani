#include <stdio.h>
#include <malloc.h>

#include "common/common_def.h"
#include "vulkan/queue_types.h"

#include "vulkan_device_queue.h"

typedef struct __queue_family_ctx {
    uint32_t family_idx;
    uint32_t queue_count;
    uint32_t state;
    VkQueue *p_queues;
    float *p_priorities;
} queue_family_ctx_t;

static struct {
    VkPhysicalDevice *p_phydev;
    queue_family_ctx_t queue_families[NUM_VULKAN_DEVICE_QUEUE_TYPES];
} queue_ctx;

void device_queue_init_ctx(void)
{
    queue_ctx.p_phydev = NULL;

    for (uint32_t idx = 0; idx < NUM_VULKAN_DEVICE_QUEUE_TYPES; idx++) {
        queue_ctx.queue_families[idx].family_idx = 0;
        queue_ctx.queue_families[idx].queue_count = 0;
        queue_ctx.queue_families[idx].state = VULKAN_DEVICE_QUEUE_STATE_UNUSED;
        queue_ctx.queue_families[idx].p_queues = NULL;
        queue_ctx.queue_families[idx].p_priorities = NULL;
    }
}

static uint32_t __device_queue_add_queue_family_ctx(VkQueueFamilyProperties *p_property,
                                                    uint32_t queue_family_count, uint32_t type)
{
    uint32_t queue_flag;
    queue_family_ctx_t *p_family_ctx;

    p_family_ctx = &queue_ctx.queue_families[type];

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
        if (!(p_property->queueFlags & queue_flag)) {
            continue;
        }

        p_family_ctx->family_idx = family_idx;
        p_family_ctx->queue_count = p_property->queueCount;

        p_family_ctx->p_priorities = (float *)malloc(sizeof(float) * p_family_ctx->queue_count);
        for (uint32_t prio_idx = 0; prio_idx < p_family_ctx->queue_count; prio_idx++) {
            p_family_ctx->p_priorities[prio_idx] = 1.0f;
        }

        return SUCCESS;
    }

    return FAILURE;
}

uint32_t device_queue_enable_queue_family(uint32_t type, VkPhysicalDevice *p_phydev)
{
    uint32_t res;
    uint32_t queue_family_count;
    queue_family_ctx_t *p_family_ctx;

    if (type >= NUM_VULKAN_DEVICE_QUEUE_TYPES) {
        return FAILURE;
    }

    p_family_ctx = &queue_ctx.queue_families[type];

    if (queue_ctx.p_phydev == NULL) {
        queue_ctx.p_phydev = p_phydev;
    }
    else if (queue_ctx.p_phydev != p_phydev) {
        printf("another physical device is already registered\n");
        return FAILURE;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_family_count, NULL);

    VkQueueFamilyProperties families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_family_count, &families);

    res = __device_queue_add_queue_family_ctx(families, queue_family_count, type);

    p_family_ctx->state = VULKAN_DEVICE_QUEUE_STATE_USED;

    return res;
}

uint32_t device_queue_get_queue_count(uint32_t type)
{
    return queue_ctx.queue_families[type].queue_count;
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

static void __device_queue_setup_default_queue_ctx(VkDeviceQueueCreateInfo *p_queue_info)
{
    p_queue_info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    p_queue_info->pNext = NULL;
    p_queue_info->flags = 0;
}

uint32_t device_queue_setup_queue_ctx(VkDeviceQueueCreateInfo *p_queues_info, uint32_t info_count)
{
    uint32_t queue_info_count;
    queue_family_ctx_t *p_families_ctx;

    p_families_ctx = queue_ctx.queue_families;
    queue_info_count = device_queue_get_total_queue_family_count();

    if (queue_info_count != info_count) {
        printf("queue info count mismatch: %u != %u\n", queue_info_count, info_count);
        return FAILURE;
    }

    for (uint32_t idx = 0; idx < queue_info_count; idx++) {
        __device_queue_setup_default_queue_ctx(&p_queues_info[idx]);

        if (p_families_ctx[idx].state == VULKAN_DEVICE_QUEUE_STATE_USED) {
            p_queues_info[idx].queueFamilyIndex = p_families_ctx[idx].family_idx;
            p_queues_info[idx].queueCount = p_families_ctx[idx].queue_count;
            p_queues_info[idx].pQueuePriorities = p_families_ctx[idx].p_priorities;
        }
    }

    return SUCCESS;
}

uint32_t device_queue_get_queue_family_idx(uint32_t type)
{
    return queue_ctx.queue_families[type].family_idx;
}
