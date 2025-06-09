#include <malloc.h>
#include <strings.h>

#include "common/common_def.h"

#include "vulkan_device.h"

#define NUM_TARGET_DEVICE_QUEUES        (3)

typedef struct capabilities {
    uint32_t queue_info_count;
    VkQueueFamilyProperties *p_queue_info;

    VkPhysicalDeviceFeatures feature;
    VkPhysicalDeviceProperties property;
} phydev_capabilities_t;

typedef struct physical_device_ctx {
    uint32_t count;
    VkPhysicalDevice *p_devices;
    phydev_capabilities_t *p_capabilities;
} phydev_ctx_t;

typedef struct logical_device_ctx {
    VkDevice device;
    uint32_t status;
    uint32_t phydev_idx;
} dev_ctx_t;

struct device_ctx_t {
    // physical device capabilities
    phydev_ctx_t phydev;

    // logical device
    dev_ctx_t dev;
} device_ctx;

VkPhysicalDevice *device_create_physical_device_list(uint32_t count)
{
    device_ctx.phydev.count = count;
    device_ctx.phydev.p_devices = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * count);

    device_ctx.dev.phydev_idx = 0;
    device_ctx.dev.status = VULKAN_DEVICE_CREATION_STATE_DEFAULT;

    return device_ctx.phydev.p_devices;
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

void device_register_physical_device_capabilities(void)
{
    VkPhysicalDevice *p_phydev;
    phydev_capabilities_t *p_capability;
    VkQueueFamilyProperties *p_queues;

    p_capability = (phydev_capabilities_t *)malloc(sizeof(phydev_capabilities_t)
                                                        * device_ctx.phydev.count);

    p_phydev = & device_ctx.phydev.p_devices[0];
    p_capability = & device_ctx.phydev.p_capabilities[0];

    for (uint32_t idx = 0; idx < device_ctx.phydev.count; idx++) {
        vkGetPhysicalDeviceProperties(*p_phydev, &p_capability->property);
        vkGetPhysicalDeviceFeatures(*p_phydev, &p_capability->feature);

        p_capability->queue_info_count = __device_create_queue_family_list(p_phydev,
                                                            p_capability->p_queue_info);

        p_phydev++;
        p_capability++;
    }
}

uint32_t get_device_count(void)
{
    return device_ctx.phydev.count;
}

static uint32_t __device_get_suitable_queue_family(phydev_capabilities_t *p_capability,
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
    phydev_capabilities_t *p_capability;
    VkDeviceQueueCreateInfo *p_queue_create_infos;
    uint32_t queue_type_flags[NUM_TARGET_DEVICE_QUEUES] = {VK_QUEUE_GRAPHICS_BIT,
                                                            VK_QUEUE_TRANSFER_BIT,
                                                            VK_QUEUE_SPARSE_BINDING_BIT};

    p_capability = &device_ctx.phydev.p_capabilities[phydev_idx];

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

    res = vkCreateDevice(device_ctx.phydev.p_devices[phydev_idx], &device_create_info,
                                                                NULL, &device_ctx.dev.device);

    if (res == VK_SUCCESS) {
        device_ctx.dev.phydev_idx = phydev_idx;
        device_ctx.dev.status = VULKAN_DEVICE_CREATION_STATE_CREATED;
    }
}

//bool vulkanDevice::isDeviceTypeGPU(uint32_t deviceIdx)
//{
//    VkPhysicalDeviceFeatures deviceFeatures;
//    VkPhysicalDeviceProperties deviceProperty;
//
//    vkGetPhysicalDeviceFeatures(pPhysicalDeviceList[deviceIdx], &deviceFeatures);
//
//    vkGetPhysicalDeviceProperties(pPhysicalDeviceList[deviceIdx], &deviceProperty);
//
//    return (deviceProperty.deviceType == 
//            (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader));
//}
//
//void vulkanDevice::selectDevice(uint32_t deviceIdx)
//{
//    currentName = deviceNamesList[deviceIdx];
//    vkGetPhysicalDeviceFeatures(pPhysicalDeviceList[deviceIdx], &feature);
//    vkGetPhysicalDeviceProperties(pPhysicalDeviceList[deviceIdx], &property);
//}
