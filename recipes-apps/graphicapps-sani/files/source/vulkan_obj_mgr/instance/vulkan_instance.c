#include <malloc.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_instance.h"

struct instance_ctx_t {
    uint32_t creation_state;

    VkResult result;

    VkInstance instance;
    VkApplicationInfo apps_info;
    VkInstanceCreateInfo creation_info;
} instance_ctx;

uint32_t instance_check_creation_state(void)
{
    return instance_ctx.creation_state;
}

static void __init_apps_info(const char *app_name)
{
    VkApplicationInfo *p_apps_info;
    p_apps_info = &(instance_ctx.apps_info);

    p_apps_info->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    p_apps_info->pNext = NULL;

    p_apps_info->pApplicationName = app_name;
    p_apps_info->pEngineName = "No Engine";

    p_apps_info->applicationVersion = VK_MAKE_VERSION(1,0,0);
    p_apps_info->engineVersion = VK_MAKE_VERSION(1, 0, 0);
    p_apps_info->apiVersion = VK_API_VERSION_1_0;
}

static void __init_creation_info(void)
{
    VkInstanceCreateInfo *p_creation_info;
    p_creation_info = &instance_ctx.creation_info;

    p_creation_info->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    p_creation_info->pNext = NULL;
    p_creation_info->pApplicationInfo = &instance_ctx.apps_info;

    p_creation_info->enabledLayerCount = 0;
    p_creation_info->enabledExtensionCount = 0;

    p_creation_info->ppEnabledLayerNames = NULL;
    p_creation_info->ppEnabledExtensionNames = NULL;
}

void instance_init(const char *app_name)
{
    __init_apps_info(app_name);

    __init_creation_info();

    instance_ctx.creation_state = VULKAN_INSTANCE_CREATION_STATE_DEFAULT;
}

void instance_add_layer_info(uint32_t layers_count, char **pp_layers_name)
{
    instance_ctx.creation_info.enabledLayerCount = layers_count;
    instance_ctx.creation_info.ppEnabledLayerNames = (const char *const *)pp_layers_name;
}

void instance_add_extension_info(uint32_t extension_count, char **pp_extensions_name)
{
    instance_ctx.creation_info.enabledExtensionCount = extension_count;
    instance_ctx.creation_info.ppEnabledExtensionNames = (const char *const *)pp_extensions_name;
}

VkResult instance_create(void)
{
    VkResult res;

    res = vkCreateInstance(&instance_ctx.creation_info, NULL, &instance_ctx.instance);

    if (res == VK_SUCCESS) {
        instance_ctx.creation_state = VULKAN_INSTANCE_CREATION_STATE_CREATED;
    }

    return res;
}

VkInstance *instance_get_instance_object(void)
{
    return &instance_ctx.instance;
}

void instance_destroy(void)
{
    vkDestroyInstance(instance_ctx.instance, NULL);
    instance_ctx.creation_state = VULKAN_INSTANCE_CREATION_STATE_DELETED;
}

uint32_t instance_get_physical_devices_count(void)
{
    uint32_t deviceCount;
    VkPhysicalDevice * deviceList;

    deviceCount = 0;

    vkEnumeratePhysicalDevices(instance_ctx.instance, &deviceCount, NULL);

    return deviceCount;
}

VkPhysicalDevice *instance_create_physical_device_list(uint32_t device_count)
{
    VkPhysicalDevice *p_dev_list;

    if (instance_ctx.creation_state != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        return NULL;
    }

    p_dev_list = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * device_count);

    vkEnumeratePhysicalDevices(instance_ctx.instance, &device_count, p_dev_list);

    return p_dev_list;
}
