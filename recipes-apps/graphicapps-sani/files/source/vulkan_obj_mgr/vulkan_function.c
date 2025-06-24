#include <malloc.h>
#include <string.h>

#include "common/common_def.h"

#include "device/vulkan_device.h"
#include "instance/vulkan_instance.h"

#include "vulkan_function.h"

typedef struct extension_ctx {
    uint32_t state;
} extension_ctx_t;

typedef struct extension_dev_ctx {
    uint32_t num_extensions;
    extension_ctx_t *p_exts;
} extension_phydev_ctx_t;

typedef struct layer_ctx {
    uint32_t state;
} layer_ctx_t;

struct function_ctx {
    uint32_t num_layers;
    uint32_t num_extensions;
    uint32_t num_phydevs_ctx;

    layer_ctx_t *p_layers;
    extension_ctx_t *p_extensions;
    extension_phydev_ctx_t *p_phydevs_ctx;
} function_ctx;

typedef struct {
    char str[VK_MAX_EXTENSION_NAME_SIZE];
} __function_name_entry;

static void __function_init_layers_ctx(void)
{
    struct function_ctx *p_ctx;

    p_ctx = &function_ctx;

    p_ctx->num_layers = 0;

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, NULL);

    p_ctx->p_layers = (layer_ctx_t *)malloc(sizeof(layer_ctx_t) * p_ctx->num_layers);

    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        p_ctx->p_layers[idx].state = VULKAN_FUNCTION_STATE_DISABLED;
    }
}

static void __function_init_extensions_ctx(void)
{
    struct function_ctx *p_ctx;

    p_ctx = &function_ctx;

    p_ctx->num_extensions = 0;

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, NULL);

    p_ctx->p_extensions = (extension_ctx_t *)malloc(sizeof(extension_ctx_t) * p_ctx->num_extensions);

    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        p_ctx->p_extensions[idx].state = VULKAN_FUNCTION_STATE_DISABLED;
    }
}

void function_init(void)
{
    __function_init_layers_ctx();

    __function_init_extensions_ctx();
}

static void __function_init_phydev_exts_ctx(void)
{
    struct function_ctx *p_ctx;
    extension_phydev_ctx_t *p_phydevs_ctx;

    p_ctx = &function_ctx;

    p_ctx->num_phydevs_ctx = device_get_phydevs_count();

    p_ctx->p_phydevs_ctx = (extension_phydev_ctx_t *)malloc(sizeof(extension_phydev_ctx_t) *
                                                                        p_ctx->num_phydevs_ctx);

    p_phydevs_ctx = p_ctx->p_phydevs_ctx;
    for (uint32_t idx = 0; idx < p_ctx->num_phydevs_ctx; idx++) {
        VkPhysicalDevice *p_phydev;

        p_phydev = device_get_phydev_object(idx);

        vkEnumerateDeviceExtensionProperties(*p_phydev, NULL, &p_phydevs_ctx->num_extensions, NULL);

        p_phydevs_ctx->p_exts = (extension_ctx_t *)malloc(sizeof(extension_ctx_t)
                                                            * p_phydevs_ctx->num_extensions);

        for (uint32_t ext_idx = 0; ext_idx < p_phydevs_ctx->num_extensions; ext_idx++) {
            p_phydevs_ctx->p_exts[ext_idx].state = VULKAN_FUNCTION_STATE_DISABLED;
        }

        p_phydevs_ctx++;
    }
}

uint32_t function_init_phydev_extension_info(void)
{
    if (instance_check_creation_state() != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        return FAILURE;
    }

    __function_init_phydev_exts_ctx();

    return SUCCESS;
}

static uint32_t __function_find_layer_idx(const char * layer_name)
{
    uint32_t layer_idx;
    struct function_ctx *p_ctx;

    p_ctx = &function_ctx;

    VkLayerProperties p_properties[p_ctx->num_layers];

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    layer_idx = 0;
    for (; layer_idx < function_ctx.num_layers; layer_idx++) {
        if (strcmp(layer_name, p_properties[layer_idx].layerName) == 0) {
            break;
        }
    }

    return layer_idx;
}

static uint32_t __function_set_layer_state(const char * layer_name, uint32_t state)
{
    uint32_t layer_idx;
    layer_idx = __function_find_layer_idx(layer_name);

    if (layer_idx < function_ctx.num_layers) {
        function_ctx.p_layers[layer_idx].state = state;

        return SUCCESS;
    }

    return FAILURE;
}

uint32_t function_enable_layer(char * layer_name)
{
    return __function_set_layer_state(layer_name, VULKAN_FUNCTION_STATE_ENABLED);
}

uint32_t function_disable_layer(char * layer_name)
{
    uint32_t layer_idx;
    uint32_t function_state;

    layer_idx = __function_find_layer_idx(layer_name);
    function_state = function_ctx.p_extensions[layer_idx].state;

    return __function_set_layer_state(layer_name, VULKAN_FUNCTION_STATE_DISABLED);
}

uint32_t function_get_num_layers(void)
{
    return function_ctx.num_layers;
}

uint32_t function_check_layer_state(char *layer_name)
{
    uint32_t layer_idx;

    layer_idx = __function_find_layer_idx(layer_name);

    if (layer_idx < function_ctx.num_layers) {
        return function_ctx.p_layers[layer_idx].state;
    }

    return VULKAN_FUNCTION_STATE_INVALID;
}

static uint32_t __function_get_matching_layer_info_count(uint32_t state)
{
    uint32_t matching_entry_cnt;

    if (state == VULKAN_FUNCTION_STATE_DEFAULT) {
        return function_ctx.num_layers;
    }

    matching_entry_cnt = 0;
    for (uint32_t idx = 0; idx < function_ctx.num_layers; idx++) {
        if (function_ctx.p_layers[idx].state != state) {
            continue;
        }

        matching_entry_cnt++;
    }

    return matching_entry_cnt;
}

uint32_t function_get_layers_count(uint32_t state)
{
    return __function_get_matching_layer_info_count(state);
}

void function_get_layers_name_list(uint32_t state, char **names_list)
{
    struct layer_ctx *p_ctx;
    __function_name_entry *p_list_entry;

    p_ctx = function_ctx.p_layers;

    p_list_entry = (__function_name_entry *)names_list;

    VkLayerProperties p_properties[function_ctx.num_layers];

    vkEnumerateInstanceLayerProperties(&function_ctx.num_layers, p_properties);

    for (uint32_t idx = 0; idx < function_ctx.num_layers; idx++) {
        if ((state == p_ctx[idx].state) || (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            memcpy(p_list_entry, p_properties[idx].layerName,
                                        strlen(p_properties[idx].layerName) + 1);

            p_list_entry++;
        }

        p_ctx++;
    }
}

static uint32_t __function_find_extension_idx(char * extension_name)
{
    struct function_ctx *p_ctx;
    uint32_t ext_idx;

    p_ctx = &function_ctx;

    VkExtensionProperties p_properties[p_ctx->num_extensions];

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, p_properties);

    ext_idx = 0;
    for (; ext_idx < function_ctx.num_extensions; ext_idx++) {
        if (strcmp(extension_name, p_properties[ext_idx].extensionName) == 0) {
            break;
        }
    }

    return ext_idx;
}

static uint32_t __function_set_extension_state(char * extension_name, uint32_t state)
{
    uint32_t ext_idx;

    ext_idx = __function_find_extension_idx(extension_name);

    if (ext_idx < function_ctx.num_extensions) {
        function_ctx.p_extensions[ext_idx].state = state;
        return SUCCESS;
    }

    return FAILURE;
}

static uint32_t __function_get_matching_extension_info_count(uint32_t state)
{
    uint32_t matching_entry_cnt;

    if (state == VULKAN_FUNCTION_STATE_DEFAULT) {
        return function_ctx.num_extensions;
    }

    matching_entry_cnt = 0;
    for (uint32_t idx = 0; idx < function_ctx.num_extensions; idx++) {
        if (function_ctx.p_extensions[idx].state != state) {
            continue;
        }

        matching_entry_cnt++;
    }

    return matching_entry_cnt;
}

uint32_t function_enable_extension(char *extension_name)
{
    return __function_set_extension_state(extension_name, VULKAN_FUNCTION_STATE_ENABLED);
}

uint32_t function_disable_extension(char *extension_name)
{
    uint32_t ext_idx;
    uint32_t function_state;

    ext_idx = __function_find_extension_idx(extension_name);
    function_state = function_ctx.p_extensions[ext_idx].state;

    return __function_set_extension_state(extension_name, VULKAN_FUNCTION_STATE_DISABLED);
}

uint32_t function_get_num_extensions(void)
{
    return function_ctx.num_extensions;
}

uint32_t function_check_extension_state(char *extension_name)
{
    uint32_t extension_idx;

    extension_idx = __function_find_extension_idx(extension_name);

    if (extension_idx < function_ctx.num_extensions) {
        return function_ctx.p_extensions[extension_idx].state;
    }

    return VULKAN_FUNCTION_STATE_INVALID;
}

uint32_t function_get_extensions_count(uint32_t state)
{
    return __function_get_matching_extension_info_count(state);
}

void function_get_extensions_name_list(uint32_t state, char **names_list)
{
    extension_ctx_t *p_ctx;
    __function_name_entry *p_list_entry;

    p_ctx = function_ctx.p_extensions;
    p_list_entry = (__function_name_entry *)names_list;

    VkExtensionProperties p_properties[function_ctx.num_extensions];

    vkEnumerateInstanceExtensionProperties(NULL, &function_ctx.num_extensions, p_properties);

    for (uint32_t idx = 0; idx < function_ctx.num_extensions; idx++) {
        if ((state == p_ctx[idx].state) || (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            memcpy(p_list_entry, p_properties[idx].extensionName,
                                strlen(p_properties[idx].extensionName) + 1);

            p_list_entry++;
        }
    }
}

static uint32_t __function_find_phydev_ext_idx(char *extension_name, uint32_t phydev_idx)
{
    uint32_t ext_idx;
    VkPhysicalDevice *p_phydev;
    extension_phydev_ctx_t *p_phydev_ctx;

    p_phydev_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    p_phydev = device_get_phydev_object(phydev_idx);

    VkExtensionProperties p_properties[p_phydev_ctx->num_extensions];

    vkEnumerateDeviceExtensionProperties(*p_phydev, NULL, &p_phydev_ctx->num_extensions, p_properties);

    ext_idx = 0;
    for (; ext_idx < function_ctx.num_extensions; ext_idx++) {
        if (strcmp(extension_name, p_properties[ext_idx].extensionName) == 0) {
            break;
        }
    }

    return ext_idx;
}

static uint32_t __function_set_phydev_extension_state(char *extension_name,
                                                        uint32_t phydev_idx, uint32_t state)
{
    uint32_t ext_idx;
    extension_phydev_ctx_t *p_phydev_ctx;

    p_phydev_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    ext_idx = __function_find_phydev_ext_idx(extension_name, phydev_idx);

    if (ext_idx < p_phydev_ctx->num_extensions) {
        p_phydev_ctx->p_exts[ext_idx].state = state;
        return SUCCESS;
    }

    return FAILURE;
}

static uint32_t __function_get_matching_phydev_extension_info_count(uint32_t state,
                                                                    uint32_t phydev_idx)
{
    uint32_t matching_entry_cnt;
    extension_phydev_ctx_t *p_phydev_ctx;

    p_phydev_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    if (state == VULKAN_FUNCTION_STATE_DEFAULT) {
        return p_phydev_ctx->num_extensions;
    }

    matching_entry_cnt = 0;
    for (uint32_t idx = 0; idx < p_phydev_ctx->num_extensions; idx++) {
        if (function_ctx.p_phydevs_ctx[phydev_idx].p_exts[idx].state != state) {
            continue;
        }

        matching_entry_cnt++;
    }

    return matching_entry_cnt;
}


uint32_t function_enable_phydev_extension(char *extension_name, uint32_t phydev_idx)
{
    return __function_set_phydev_extension_state(extension_name, phydev_idx,
                                                    VULKAN_FUNCTION_STATE_ENABLED);
}

uint32_t function_disable_phydev_extension(char *extension_name, uint32_t phydev_idx)
{
    return __function_set_phydev_extension_state(extension_name, phydev_idx,
                                                    VULKAN_FUNCTION_STATE_DISABLED);
}

static void __function_get_phydev_extensions_list(uint32_t phydev_idx,
                                                    VkExtensionProperties *list)
{
    VkPhysicalDevice *p_phydev;
    extension_phydev_ctx_t *p_exts_ctx;

    p_phydev = device_get_phydev_object(phydev_idx);

    p_exts_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    vkEnumerateDeviceExtensionProperties(*p_phydev, NULL, &p_exts_ctx->num_extensions, list);
    return;
}

void function_get_phydev_exts_name_list(uint32_t state, char **names_list, uint32_t phydev_idx)
{
    extension_phydev_ctx_t *p_ctx;
    __function_name_entry *p_list_entry;

    p_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];
    p_list_entry = (__function_name_entry *)names_list;

    VkExtensionProperties p_properties[p_ctx->num_extensions];

    __function_get_phydev_extensions_list(phydev_idx, p_properties);

    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        if ((state == p_ctx->p_exts[idx].state) || (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            memcpy(p_list_entry, p_properties[idx].extensionName,
                                        strlen(p_properties[idx].extensionName) + 1);
            p_list_entry++;
        }
    }
}

uint32_t function_get_phydev_exts_count(uint32_t state, uint32_t phydev_idx)
{
    return __function_get_matching_phydev_extension_info_count(state, phydev_idx);
}
