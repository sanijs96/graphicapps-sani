#include <malloc.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_instance.h"

typedef struct extension_properties {
    uint32_t state;
    const VkExtensionProperties *p_pty;
} extension_properties_ctx_t;

typedef struct layer_properties {
    uint32_t state;
    uint32_t num_extensions;

    const VkLayerProperties *p_pty;

    extension_properties_ctx_t *p_extensions;

} layer_properties_ctx_t;

struct instance_ctx_t {
    uint32_t creation_state;

    VkResult result;

    VkInstance instance;
    VkApplicationInfo apps_info;
    VkInstanceCreateInfo creation_info;

    uint32_t num_layers;
    layer_properties_ctx_t *p_layers;

} instance_ctx;

uint32_t check_instance_state(void)
{
    return instance_ctx.creation_state;
}

static void __init_apps_info(const char *app_name)
{
    VkApplicationInfo *p_apps_info;
    p_apps_info = &(instance_ctx.apps_info);

    p_apps_info->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    p_apps_info->pApplicationName = app_name;
    p_apps_info->pEngineName = "No Engine";

    p_apps_info->applicationVersion = VK_MAKE_VERSION(1,0,0);
    p_apps_info->engineVersion = VK_MAKE_VERSION(1, 0, 0);
    p_apps_info->apiVersion = VK_API_VERSION_1_0;

    p_apps_info->pNext = NULL;
}

static void __init_creation_info(void)
{
    VkInstanceCreateInfo *p_creation_info;
    p_creation_info = &instance_ctx.creation_info;

    p_creation_info->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    p_creation_info->pApplicationInfo = &instance_ctx.apps_info;

    p_creation_info->enabledLayerCount = 0;
    p_creation_info->enabledExtensionCount = 0;

    p_creation_info->ppEnabledLayerNames = NULL;
    p_creation_info->ppEnabledExtensionNames = NULL;
}

static void __setup_available_extensions_ctx_per_layer(layer_properties_ctx_t *p_layer_ctx)
{
    const char *layer_name;
    VkExtensionProperties *p_properties;
    extension_properties_ctx_t *p_extensions;

    layer_name = p_layer_ctx->p_pty->layerName;
    p_properties = NULL;
    p_extensions = p_layer_ctx->p_extensions;

    vkEnumerateInstanceExtensionProperties(layer_name, &p_layer_ctx->num_extensions, p_properties);

    p_properties = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties)
                                                    * p_layer_ctx->num_extensions);
    p_extensions = (extension_properties_ctx_t *)malloc(sizeof(extension_properties_ctx_t)
                                                            * p_layer_ctx->num_extensions);
    vkEnumerateInstanceExtensionProperties(layer_name, &p_layer_ctx->num_extensions, p_properties);

    for (uint32_t idx = 0; idx < p_layer_ctx->num_extensions; idx++) {
        p_extensions[idx].p_pty = &p_properties[idx];
        p_extensions[idx].state = VULKAN_INSTANCE_FUNCTION_STATE_DISABLED;
    }
}

static void __setup_available_layers_ctx(void)
{
    struct instance_ctx_t *p_ctx;
    VkLayerProperties *p_properties;
    layer_properties_ctx_t *p_layers;

    p_ctx = &instance_ctx;

    p_ctx->num_layers = 0;
    p_properties = NULL;
    p_layers = p_ctx->p_layers;

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    p_properties = (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * p_ctx->num_layers);
    p_layers = (layer_properties_ctx_t *)malloc(sizeof(layer_properties_ctx_t) * p_ctx->num_layers);

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        p_layers[idx].p_pty = &p_properties[idx];
        p_layers[idx].state = VULKAN_INSTANCE_FUNCTION_STATE_DISABLED;

        __setup_available_extensions_ctx_per_layer(&p_layers[idx]);
    }
}

void instance_init_ctx(const char *app_name)
{
    __init_apps_info(app_name);

    __init_creation_info();

    __setup_available_layers_ctx();

    instance_ctx.creation_state = VULKAN_INSTANCE_CREATION_STATE_DEFAULT;
}

VkResult instance_create(void)
{
    vkCreateInstance(&instance_ctx.creation_info, NULL, &instance_ctx.instance);
}

VkResult instance_destroy(void)
{
    vkDestroyInstance(instance_ctx.instance, NULL);
}

static uint32_t __instance_find_layer_idx(const char * layer_name)
{
    uint32_t layer_idx;
    layer_properties_ctx_t *p_layer_ctx;

    p_layer_ctx = &instance_ctx.p_layers[0];

    layer_idx = 0;
    for (; layer_idx < instance_ctx.num_layers; layer_idx++) {
        if (strcmp(layer_name, p_layer_ctx->p_pty->layerName) == 0) {
            break;
        }

        p_layer_ctx++;
    }

    return layer_idx;
}

static uint32_t __instance_set_layer_state(const char * layer_name, uint32_t state)
{
    uint32_t layer_idx;
    layer_idx = __instance_find_layer_idx(layer_name);

    if (layer_idx < instance_ctx.num_layers) {
        instance_ctx.p_layers[layer_idx].state = state;

        return SUCCESS;
    }

    return FAILURE;
}

uint32_t instance_enable_layer(char * layer_name)
{
    return __instance_set_layer_state(layer_name, VULKAN_INSTANCE_FUNCTION_STATE_ENABLED);
}

uint32_t instance_disable_layer(char * layer_name)
{
    return __instance_set_layer_state(layer_name, VULKAN_INSTANCE_FUNCTION_STATE_DISABLED);
}

static uint32_t __instance_find_extension_idx(layer_properties_ctx_t *p_layer_ctx,
                                                                char * extension_name)
{
    extension_properties_ctx_t *p_extension_ctx;
    uint32_t ext_idx;
    ext_idx = 0;

    p_extension_ctx = &p_layer_ctx->p_extensions[0];
    for (;ext_idx < p_layer_ctx->num_extensions; ext_idx++) {
        if (!strcmp(extension_name, p_extension_ctx->p_pty->extensionName)) {
            break;
        }
        p_extension_ctx++;
    }

    return ext_idx;
}

static uint32_t __instance_find_extension_layer_idx(char * extension_name)
{
    uint32_t layer_idx;
    layer_properties_ctx_t *p_layer_ctx;
    extension_properties_ctx_t *p_extension_ctx;

    p_layer_ctx = &instance_ctx.p_layers[0];

    layer_idx = 0;
    for (uint32_t ext_idx = 0; layer_idx < instance_ctx.num_layers; layer_idx++) {
        ext_idx = __instance_find_extension_idx(p_layer_ctx, extension_name);
        if (ext_idx < p_layer_ctx->num_extensions) {
            break;
        }

        p_layer_ctx++;
    }

    return layer_idx;
}

static uint32_t __instance_get_extension_state(layer_properties_ctx_t *p_layer_ctx,
                                                                char * extension_name)
{
    uint32_t ext_idx;
    ext_idx = __instance_find_extension_idx(p_layer_ctx, extension_name);

    return p_layer_ctx->state;
}

static uint32_t __instance_set_extension_state(layer_properties_ctx_t *p_layer_ctx,
                                                char * extension_name, uint32_t state)
{
    uint32_t ext_idx;
    ext_idx = __instance_find_extension_idx(p_layer_ctx, extension_name);

    if (ext_idx < p_layer_ctx->num_extensions) {
        p_layer_ctx->state = state;

        return SUCCESS;
    }

    return FAILURE;
}

uint32_t instance_enable_extension(char *extension_name)
{
    uint32_t extension_layer_idx;
    layer_properties_ctx_t *p_layer_ctx;

    // find parent layer of targetted extension
    extension_layer_idx = __instance_find_extension_layer_idx(extension_name);

    if (extension_layer_idx < instance_ctx.num_layers) {
        p_layer_ctx = &instance_ctx.p_layers[extension_layer_idx];
    }
    else {
        return FAILURE;
    }

    return __instance_set_extension_state(p_layer_ctx, extension_name,
                                VULKAN_INSTANCE_FUNCTION_STATE_ENABLED);
}

uint32_t instance_disable_extension(char *extension_name)
{
    uint32_t extension_layer_idx;
    layer_properties_ctx_t *p_layer_ctx;

    // find parent layer of targetted extension
    extension_layer_idx = __instance_find_extension_layer_idx(extension_name);

    if (extension_layer_idx < instance_ctx.num_layers) {
        p_layer_ctx = &instance_ctx.p_layers[extension_layer_idx];
    }
    else {
        return FAILURE;
    }

    return __instance_set_extension_state(p_layer_ctx, extension_name,
                                VULKAN_INSTANCE_FUNCTION_STATE_DISABLED);
}

uint32_t check_instance_extension_state(char *extension_name)
{
    uint32_t extension_layer_idx;
    layer_properties_ctx_t *p_layer_ctx;

    // find parent layer of targetted extension
    extension_layer_idx = __instance_find_extension_layer_idx(extension_name);

    if (extension_layer_idx < instance_ctx.num_layers) {
        p_layer_ctx = &instance_ctx.p_layers[extension_layer_idx];
    }
    else {
        return FAILURE;
    }

    return __instance_get_extension_state(p_layer_ctx, extension_name);
}

uint32_t instance_get_physical_devices_count(void)
{
    uint32_t deviceCount;
    VkPhysicalDevice * deviceList;

    deviceCount = 0;

    vkEnumeratePhysicalDevices(instance_ctx.instance, &deviceCount, NULL);

    return deviceCount;
}

void instance_setup_physical_device_lists(VkPhysicalDevice *p_dev_list, uint32_t device_count)
{
    vkEnumeratePhysicalDevices(instance_ctx.instance, &device_count, p_dev_list);
}
