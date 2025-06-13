#include <malloc.h>
#include <strings.h>

#include "common/common_def.h"

#include "vulkan_device.h"

typedef struct __phydev_capability {
    uint32_t queue_info_count;
    VkQueueFamilyProperties *p_queue_infos;
    VkPhysicalDeviceFeatures *p_feature;
    VkPhysicalDeviceProperties *p_property;
} pdev_capability_t;

typedef struct physical_device_ctx {
    VkPhysicalDevice *p_device;
    pdev_capability_t capability;
} pdev_ctx_t;

typedef struct __logical_device_capability {
    uint32_t queue_status;
    VkDeviceQueueCreateInfo *p_qcreate_info;
} ldev_capability_t;

typedef struct logical_device_ctx {
    uint32_t status;
    uint32_t phydev_idx;
    VkDeviceCreateInfo ldev_create_info;

    ldev_capability_t *p_capabilities;
} ldev_ctx_t;

struct device_ctx_t {
    uint32_t pdev_count;
    // physical device capabilities
    pdev_ctx_t *pdevs_ctx;

    // logical device
    ldev_ctx_t ldev_ctx;
    VkDevice *p_device_handler;
} device_ctx;

void device_create_physical_device_ctx(uint32_t count)
{
    device_ctx.pdev_count = count;
    device_ctx.pdevs_ctx = (pdev_ctx_t *)malloc(sizeof(pdev_ctx_t) * count);

    device_ctx.ldev_ctx.phydev_idx = 0;
    device_ctx.ldev_ctx.status = VULKAN_DEVICE_CREATION_STATE_DEFAULT;
}

void device_register_physical_device(VkPhysicalDevice *p_device, uint32_t phydev_idx)
{
    if (phydev_idx >= device_ctx.pdev_count) {
        return;
    }
    device_ctx.pdevs_ctx[phydev_idx].p_device = p_device;
}

static void __device_setup_queue_family_ctx(VkPhysicalDevice *p_phydev, uint32_t phydev_idx)
{
    uint32_t queue_info_count;
    ldev_capability_t *p_ldev_caps;
    VkQueueFamilyProperties *p_queues;

    queue_info_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_info_count, NULL);

    p_queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) *
                                                                    queue_info_count);
    vkGetPhysicalDeviceQueueFamilyProperties(*p_phydev, &queue_info_count, p_queues);

    device_ctx.pdevs_ctx[phydev_idx].capability.queue_info_count = queue_info_count;
    device_ctx.pdevs_ctx[phydev_idx].capability.p_queue_infos = p_queues;

    return;
}

static void __device_setup_default_ldev_queue_info(uint32_t queue_info_idx,
                                                    pdev_capability_t *p_pdev_cap)
{
    VkQueueFamilyProperties *p_pdev_queue_info;
    VkDeviceQueueCreateInfo *p_ldev_qcreate_info;

    p_pdev_queue_info = &p_pdev_cap->p_queue_infos[queue_info_idx];

    p_ldev_qcreate_info = device_ctx.ldev_ctx.p_capabilities[queue_info_idx].p_qcreate_info;

    p_ldev_qcreate_info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    p_ldev_qcreate_info->queueFamilyIndex = queue_info_idx;
    p_ldev_qcreate_info->queueCount = p_pdev_queue_info->queueCount;
    p_ldev_qcreate_info->flags = 0;

    p_ldev_qcreate_info->pQueuePriorities = (const float *){0, };
}

static void __device_setup_ldev_queue_ctx(uint32_t phydev_idx)
{
    uint32_t queue_info_count;
    pdev_capability_t *p_pdev_cap;
    ldev_capability_t *p_ldev_caps;

    p_pdev_cap = &device_ctx.pdevs_ctx[phydev_idx].capability;

    device_ctx.ldev_ctx.p_capabilities = (ldev_capability_t *)
                                            malloc(sizeof(ldev_capability_t) * queue_info_count);

    p_ldev_caps = device_ctx.ldev_ctx.p_capabilities;

    for (uint32_t qinfo_idx = 0; qinfo_idx < p_pdev_cap->queue_info_count; qinfo_idx++) {
        p_ldev_caps[qinfo_idx].p_qcreate_info = (VkDeviceQueueCreateInfo *)
                                                    malloc(sizeof(VkDeviceQueueCreateInfo));

        __device_setup_default_ldev_queue_info(qinfo_idx, p_pdev_cap);

        p_ldev_caps[qinfo_idx].queue_status = VULKAN_DEVICE_QUEUE_UNUSED;
    }
}

void device_register_device_capability(uint32_t phydev_idx)
{
    VkPhysicalDevice *p_phydev;
    pdev_capability_t *p_capability;

    p_capability = &device_ctx.pdevs_ctx[phydev_idx].capability;

    p_capability->p_feature = (VkPhysicalDeviceFeatures *)
                                malloc(sizeof(VkPhysicalDeviceFeatures));
    p_capability->p_property = (VkPhysicalDeviceProperties *)
                                malloc(sizeof(VkPhysicalDeviceProperties));

    p_phydev = device_ctx.pdevs_ctx[phydev_idx].p_device;

    vkGetPhysicalDeviceFeatures(*p_phydev, p_capability->p_feature);
    vkGetPhysicalDeviceProperties(*p_phydev, p_capability->p_property);

    __device_setup_queue_family_ctx(p_phydev, phydev_idx);
    __device_setup_ldev_queue_ctx(phydev_idx);
}

uint32_t device_get_num_phydevs(void)
{
    return device_ctx.pdev_count;
}

// TODO: support queue customization(using queue_info)
static void __device_setup_ldev_queue_create_ctx(VkDeviceCreateInfo *p_ldev_create_info,
                                                                    uint32_t phydev_idx)
{
    uint32_t queue_type_flags;
    pdev_capability_t *p_pdev_cap;
    ldev_capability_t *p_ldev_cap;
    VkQueueFamilyProperties *p_pdev_queue_info;
    VkDeviceQueueCreateInfo *p_ldev_qcreate_info;

    queue_type_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT;

    p_pdev_cap = &device_ctx.pdevs_ctx[phydev_idx].capability;
    p_ldev_cap = device_ctx.ldev_ctx.p_capabilities;

    for (uint32_t qinfo_idx = 0; qinfo_idx < p_pdev_cap->queue_info_count; qinfo_idx++) {
        p_pdev_queue_info = &p_pdev_cap->p_queue_infos[qinfo_idx];
        p_ldev_qcreate_info = p_ldev_cap[qinfo_idx].p_qcreate_info;

        // find suitable queues
        if ((p_pdev_queue_info->queueFlags & queue_type_flags) == queue_type_flags) {
            p_ldev_qcreate_info->queueCount = p_pdev_queue_info->queueCount;

            p_ldev_create_info->queueCreateInfoCount++;

            p_ldev_cap[qinfo_idx].queue_status = VULKAN_DEVICE_QUEUE_USED;
        }
    }
}

void __device_include_target_queue_create_ctx(VkDeviceQueueCreateInfo *p_qcreate_infos,
                                                                uint32_t max_queue_info_count)
{
    ldev_capability_t *p_ldev_caps;

    p_ldev_caps = device_ctx.ldev_ctx.p_capabilities;

    for (uint32_t idx = 0; idx < max_queue_info_count; idx++) {
        if (p_ldev_caps[idx].queue_status != VULKAN_DEVICE_QUEUE_USED) {
            continue;
        }

        *p_qcreate_infos = *p_ldev_caps[idx].p_qcreate_info;
        p_qcreate_infos++;
    }
}

// TODO: enable device layer & extension
VkResult device_create(uint32_t phydev_idx)
{
    VkResult res;
    uint32_t queue_info_count;
    VkDevice *p_device_handler;
    VkDeviceCreateInfo *p_ldev_create_info;

    if (device_get_current_status == VULKAN_DEVICE_CREATION_STATE_CREATED) {
        return FAILURE;
    }

    queue_info_count = device_get_num_device_queue_properties(phydev_idx);

    p_ldev_create_info = &device_ctx.ldev_ctx.ldev_create_info;

    p_ldev_create_info->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    p_ldev_create_info->pNext = NULL;

    __device_setup_ldev_queue_create_ctx(p_ldev_create_info, phydev_idx);

    uint32_t max_qcreate_info_count;
    max_qcreate_info_count = device_ctx.pdevs_ctx[phydev_idx].capability.queue_info_count;

    uint32_t qcreate_info_count;
    qcreate_info_count = p_ldev_create_info->queueCreateInfoCount;
    VkDeviceQueueCreateInfo p_qcreate_infos[qcreate_info_count];

    __device_include_target_queue_create_ctx(p_qcreate_infos, max_qcreate_info_count);

    p_ldev_create_info->pQueueCreateInfos = p_qcreate_infos;

    p_ldev_create_info->enabledExtensionCount = 0;
    p_ldev_create_info->enabledLayerCount = 0;
    p_ldev_create_info->ppEnabledExtensionNames = NULL;
    p_ldev_create_info->pEnabledFeatures = NULL;
    p_ldev_create_info->ppEnabledLayerNames = NULL;

    p_device_handler = (VkDevice *)malloc(sizeof(VkDevice));

    res = vkCreateDevice(*device_ctx.pdevs_ctx[phydev_idx].p_device, p_ldev_create_info,
                                                                    NULL, p_device_handler);
    if (res == VK_SUCCESS) {
        device_ctx.ldev_ctx.phydev_idx = phydev_idx;
        device_ctx.ldev_ctx.status = VULKAN_DEVICE_CREATION_STATE_CREATED;
        device_ctx.p_device_handler = p_device_handler;
    }
    else {
        free(p_device_handler);
    }

    printf("res %u\n", res);


    return res;
}

uint32_t device_get_current_status(void)
{
    return device_ctx.ldev_ctx.status;
}

uint32_t device_get_current_phydev_idx(void)
{
    return device_ctx.ldev_ctx.phydev_idx;
}

VkPhysicalDeviceProperties *device_get_device_property(uint32_t phydev_idx)
{
    return device_ctx.pdevs_ctx[phydev_idx].capability.p_property;
}

uint32_t device_get_num_device_queue_properties(uint32_t phydev_idx)
{
    return device_ctx.pdevs_ctx[phydev_idx].capability.queue_info_count;
}


VkQueueFamilyProperties *device_get_device_queue_property(uint32_t phydev_idx)
{
    return device_ctx.pdevs_ctx[phydev_idx].capability.p_queue_infos;
}
