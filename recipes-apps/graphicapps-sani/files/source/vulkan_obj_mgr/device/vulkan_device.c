#include <malloc.h>
#include <strings.h>

#include "common/common_def.h"

#include "vulkan_device.h"

#define NUM_TARGET_DEVICE_QUEUES        (3)

typedef struct capability {
    uint32_t queue_info_count;
    VkQueueFamilyProperties *p_queue_info;

    VkPhysicalDeviceFeatures feature;
    VkPhysicalDeviceProperties property;
} phydev_capability_t;

typedef struct physical_device_ctx {
    VkPhysicalDevice *p_device;
    phydev_capability_t *p_capability;
} phydev_ctx_t;

struct device_ctx_t {
    uint32_t phydev_count;
    // physical device capabilities
    phydev_ctx_t *phydevs_ctx;

    // logical device
    dev_ctx_t dev_ctx;
    VkDevice device_handler;
} device_ctx;

void device_create_physical_device_ctx(uint32_t count)
{
    device_ctx.phydev_count = count;
    device_ctx.phydevs_ctx = (phydev_ctx_t *)malloc(sizeof(phydev_ctx_t) * count);

    device_ctx.dev_ctx.phydev_idx = 0;
    device_ctx.dev_ctx.status = VULKAN_DEVICE_CREATION_STATE_DEFAULT;
}

void device_register_physical_device_handler(VkPhysicalDevice *p_handle, uint32_t phydev_idx)
{
    if (phydev_idx >= device_ctx.phydev_count) {
        return;
    }
    device_ctx.phydevs_ctx[phydev_idx].p_device = p_handle;
}

uint32_t __device_create_queue_family_list(VkPhysicalDevice *p_phydev,
                                            VkQueueFamilyProperties *p_queues)
{
    uint32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_count, NULL);

    p_queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queue_count);

    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_count, p_queues);

    return queue_count;
}

void device_register_physical_device_capability(uint32_t phydev_idx)
{
    VkPhysicalDevice *p_phydev;
    phydev_capability_t *p_capability;
    VkQueueFamilyProperties *p_queues;

    p_capability = (phydev_capability_t *)malloc(sizeof(phydev_capability_t));

    device_ctx.phydevs_ctx[phydev_idx].p_capability = &p_capability[phydev_idx];

    p_phydev = device_ctx.phydevs_ctx[phydev_idx].p_device;

    vkGetPhysicalDeviceFeatures(*p_phydev, &p_capability[phydev_idx].feature);
    vkGetPhysicalDeviceProperties(*p_phydev, &p_capability[phydev_idx].property);

    p_capability->queue_info_count = __device_create_queue_family_list(p_phydev,
                                                        p_capability->p_queue_info);
}

uint32_t device_get_num_phydevs(void)
{
    return device_ctx.phydev_count;
}

static uint32_t __device_get_suitable_queue_family(phydev_capability_t *p_capability,
                                                                        uint32_t flags_cond)
{
    uint32_t idx;
    idx = 0;
    for (; idx < NUM_TARGET_DEVICE_QUEUES; idx++) {
        if ((p_capability->p_queue_info->queueFlags & flags_cond) == flags_cond) {
            break;
        }
    }

    return idx;
}

static VkDeviceQueueCreateInfo * __device_allocate_queue_create_info(void)
{
    return malloc((sizeof(VkDeviceQueueCreateInfo) * NUM_TARGET_DEVICE_QUEUES));
}

static void __device_setup_queue_create_info(VkDeviceCreateInfo *p_dev_ctx, uint32_t phydev_idx)
{
    phydev_capability_t *p_capability;
    VkDeviceQueueCreateInfo *p_queue_create_infos;
    uint32_t queue_type_flags[NUM_TARGET_DEVICE_QUEUES] = {VK_QUEUE_GRAPHICS_BIT,
                                                            VK_QUEUE_TRANSFER_BIT,
                                                            VK_QUEUE_SPARSE_BINDING_BIT};

    p_capability = device_ctx.phydevs_ctx[phydev_idx].p_capability;

    p_queue_create_infos = __device_allocate_queue_create_info();

    for (uint32_t idx = 0; idx < NUM_TARGET_DEVICE_QUEUES; idx++) {
        uint32_t queue_family_idx;

        queue_family_idx = __device_get_suitable_queue_family(p_capability, queue_type_flags[idx]);

        if (queue_family_idx < p_capability->queue_info_count) {
            p_dev_ctx->queueCreateInfoCount++;
            p_queue_create_infos[idx].queueFamilyIndex = queue_family_idx;
        }
    }
}

// TODO: support queue customization(using queue_info)
VkResult device_create(uint32_t phydev_idx, device_queue_ctx_t queue_info)
{
    VkResult res;
    uint32_t *queue_type_flags;
    VkDeviceCreateInfo device_create_info;

    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    device_create_info.pNext = NULL;

    __device_setup_queue_create_info(&device_create_info, phydev_idx);

    // TODO: enable device layer & extension

    device_create_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(*device_ctx.phydevs_ctx[phydev_idx].p_device, &device_create_info,
                                                                NULL, &device_ctx.device_handler);

    if (res == VK_SUCCESS) {
        device_ctx.dev_ctx.phydev_idx = phydev_idx;
        device_ctx.dev_ctx.status = VULKAN_DEVICE_CREATION_STATE_CREATED;
    }
}

dev_ctx_t device_get_current_device_info(void)
{
    return device_ctx.dev_ctx;
}

VkPhysicalDeviceProperties *device_get_device_property(uint32_t phydev_idx)
{
    return &device_ctx.phydevs_ctx[phydev_idx].p_capability->property;
}

uint32_t device_get_num_device_queue_properties(uint32_t phydev_idx)
{
    return device_ctx.phydevs_ctx[phydev_idx].p_capability->queue_info_count;
}


VkQueueFamilyProperties *device_get_device_queue_property(uint32_t phydev_idx)
{
    return device_ctx.phydevs_ctx[phydev_idx].p_capability->p_queue_info;
}
