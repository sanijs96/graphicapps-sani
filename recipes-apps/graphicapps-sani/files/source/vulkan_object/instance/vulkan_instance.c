#include <malloc.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_instance.h"

static struct {
    uint32_t creation_state;

    uint32_t layer_count;
    uint32_t extension_count;

    char ** p_layers_name;
    char ** p_extensions_name;

    VkInstance instance;
} instance_ctx;

uint32_t instance_check_creation_state(void)
{
    return instance_ctx.creation_state;
}

void instance_init(void)
{
    instance_ctx.creation_state = VULKAN_INSTANCE_CREATION_STATE_DEFAULT;
}

void instance_add_layer_info(uint32_t layers_count, char **pp_layers_name)
{
    instance_ctx.layer_count = layers_count;
    instance_ctx.p_layers_name = pp_layers_name;
}

void instance_add_extension_info(uint32_t extension_count, char **pp_extensions_name)
{
    instance_ctx.extension_count = extension_count;
    instance_ctx.p_extensions_name = pp_extensions_name;
}

static void __init_apps_info(VkApplicationInfo *p_apps_info)
{
    p_apps_info->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    p_apps_info->pNext = NULL;

    p_apps_info->pApplicationName = "Sani Vulkan Application";
    p_apps_info->pEngineName = "No Engine";

    p_apps_info->applicationVersion = VK_MAKE_VERSION(1,0,0);
    p_apps_info->engineVersion = VK_MAKE_VERSION(1, 0, 0);
    p_apps_info->apiVersion = VK_API_VERSION_1_0;
}

static void __init_creation_info(VkInstanceCreateInfo *p_creation_info)
{
    p_creation_info->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    p_creation_info->pNext = NULL;
    p_creation_info->flags = 0;

    p_creation_info->enabledLayerCount = instance_ctx.layer_count;
    p_creation_info->enabledExtensionCount = instance_ctx.extension_count;

    p_creation_info->ppEnabledLayerNames = (const char *const *)instance_ctx.p_layers_name;
    p_creation_info->ppEnabledExtensionNames = (const char *const *)instance_ctx.p_extensions_name;
}

uint32_t instance_create(char **layer_names, char **extension_names)
{
    uint32_t res;
    VkApplicationInfo apps_info;
    VkInstanceCreateInfo instance_info;

    instance_add_layer_info(instance_ctx.layer_count, layer_names);
    instance_add_extension_info(instance_ctx.extension_count, extension_names);

    __init_creation_info(&instance_info);

    __init_apps_info(&apps_info);

    instance_info.pApplicationInfo = &apps_info;

    res = vkCreateInstance(&instance_info, NULL, &instance_ctx.instance);
    if (res != VK_SUCCESS) {
        printf("instance creation failure: %d\n", res);
        return FAILURE;
    }

    instance_ctx.creation_state = VULKAN_INSTANCE_CREATION_STATE_CREATED;

    return SUCCESS;
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
