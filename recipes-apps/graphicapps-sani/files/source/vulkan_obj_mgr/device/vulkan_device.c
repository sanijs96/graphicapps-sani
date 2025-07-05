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

struct device_ctx_t {
    uint32_t status;
    uint32_t pdev_count;
    VkPhysicalDevice *p_phydevs;

    VkDevice device;
    uint32_t phydev_idx;
    function_ctx_t layer_ctx;
    function_ctx_t extension_ctx;
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

uint32_t device_create(uint32_t phydev_idx)
{
    uint32_t res;
    uint32_t queue_family_idx;
    uint32_t queue_info_count;
    VkDevice *p_device_handler;
    VkPhysicalDevice *p_phydev;
    VkDeviceCreateInfo device_info;

    if (device_get_current_status() == VULKAN_DEVICE_CREATION_STATE_CREATED) {
        printf("device already created\n");
        return FAILURE;
    }

    p_phydev = &device_ctx.p_phydevs[phydev_idx];
    for (uint32_t type = 0; type < NUM_VULKAN_DEVICE_QUEUE_TYPES; type++) {
        if (device_queue_enable_queue_family(type, p_phydev) == FAILURE) {
            printf("queue type %u enable failure\n", type);
        }
    }

    queue_info_count = device_queue_get_total_queue_family_count();

    VkDeviceQueueCreateInfo queues_info[queue_info_count];

    res = device_queue_setup_queue_ctx(queues_info, queue_info_count);
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
    device_info.ppEnabledLayerNames = device_ctx.layer_ctx.names_list;

    device_info.enabledExtensionCount = device_ctx.extension_ctx.count;
    device_info.ppEnabledExtensionNames = device_ctx.extension_ctx.names_list;

    device_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(*p_phydev, &device_info, NULL, &device_ctx.device);
    if (res == VK_SUCCESS) {
        device_ctx.phydev_idx = phydev_idx;
        device_ctx.status = VULKAN_DEVICE_CREATION_STATE_CREATED;
    }
    else {
        printf("device creation failure: %d\n", res);
        free(p_device_handler);
    }

    return res;
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

uint32_t device_get_queue_idx(uint32_t queue_type)
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
