#include <malloc.h>
#include <string.h>

#include "common/common_def.h"
#include "vulkan/function_scope.h"

#include "vulkan_function.h"

typedef struct function_ctx {
    uint32_t *states;
    uint32_t enabled_count;
} function_ctx_t;

typedef struct function_setup_ctx {
    uint32_t num_layers;
    uint32_t num_extensions;
    function_ctx_t layer_ctx;
    function_ctx_t extension_ctx;
} function_setup_ctx_t;

static struct {
    uint32_t num_phydevs;

    function_setup_ctx_t *p_phydevs_ctx;
    function_setup_ctx_t instance_ctx;
} function_ctx;

static inline void __function_init_instance_ctx(void)
{
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;

    p_ctx->num_layers = 0;
    p_ctx->num_extensions = 0;

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, NULL);

    p_ctx->layer_ctx.states = (uint32_t *)malloc(sizeof(uint32_t) * p_ctx->num_layers);

    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        p_ctx->layer_ctx.states[idx] = VULKAN_FUNCTION_STATE_DISABLED;
    }

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, NULL);
    p_ctx->extension_ctx.states = (uint32_t *)malloc(sizeof(uint32_t) * p_ctx->num_extensions);

    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        p_ctx->extension_ctx.states[idx] = VULKAN_FUNCTION_STATE_DISABLED;
    }
}

void function_init_layers_ctx(void)
{
    __function_init_instance_ctx();

    function_ctx.num_phydevs = 0;
    function_ctx.p_phydevs_ctx = NULL;
}

static void __function_init_phydevs_ctx(uint32_t phydev_count, VkPhysicalDevice *p_phydevs)
{
    function_setup_ctx_t *p_phydevs_ctx;

    function_ctx.num_phydevs = phydev_count;

    function_ctx.p_phydevs_ctx = (function_setup_ctx_t *)malloc(sizeof(function_setup_ctx_t) *
                                                                                    phydev_count);

    p_phydevs_ctx = function_ctx.p_phydevs_ctx;

    for (uint32_t idx = 0; idx < phydev_count; idx++) {
        vkEnumerateDeviceLayerProperties(p_phydevs[idx], &p_phydevs_ctx[idx].num_layers, NULL);
        p_phydevs_ctx[idx].layer_ctx.states = (uint32_t *)malloc(sizeof(uint32_t) *
                                                                    p_phydevs_ctx[idx].num_layers);

        for (uint32_t layer_idx = 0; layer_idx < p_phydevs_ctx->num_layers; layer_idx++) {
            p_phydevs_ctx->layer_ctx.states[layer_idx] = VULKAN_FUNCTION_STATE_DISABLED;
        }

        vkEnumerateDeviceExtensionProperties(p_phydevs[idx], NULL,
                                                &p_phydevs_ctx[idx].num_extensions, NULL);
        p_phydevs_ctx[idx].extension_ctx.states = (uint32_t *)malloc(sizeof(uint32_t) *
                                                            p_phydevs_ctx[idx].num_extensions);

        for (uint32_t ext_idx = 0; ext_idx < p_phydevs_ctx->num_extensions; ext_idx++) {
            p_phydevs_ctx[idx].extension_ctx.states[ext_idx] = VULKAN_FUNCTION_STATE_DISABLED;
        }
    }
}

uint32_t function_init_phydevs_ctx(uint32_t phydev_count, VkPhysicalDevice *p_phydevs)
{
    __function_init_phydevs_ctx(phydev_count, p_phydevs);

    return SUCCESS;
}

static uint32_t __function_find_layer_idx(VkLayerProperties *p_list, uint32_t list_size,
                                                                    const char *layer_name)
{
    for (uint32_t layer_idx = 0; layer_idx < list_size; layer_idx++) {
        if (strcmp(layer_name, p_list[layer_idx].layerName) == 0) {
            return layer_idx;
        }
    }

    return list_size;
}

static uint32_t __function_find_instance_layer_idx(const char *layer_name)
{
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;

    VkLayerProperties p_properties[p_ctx->num_layers];

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    return __function_find_layer_idx(p_properties, p_ctx->num_layers, layer_name);
}

static inline void __function_set_layer_state(function_setup_ctx_t *p_ctx,
                                                uint32_t layer_idx, uint32_t state)
{
    p_ctx->layer_ctx.states[layer_idx] = state;
}

uint32_t function_enable_instance_layer(char *layer_name)
{
    uint32_t layer_idx;

    layer_idx = __function_find_instance_layer_idx(layer_name);
    if (layer_idx == function_ctx.instance_ctx.num_layers) {
        return FAILURE;
    }

    __function_set_layer_state(&function_ctx.instance_ctx, layer_idx,
                                        VULKAN_FUNCTION_STATE_ENABLED);

    return SUCCESS;
}

uint32_t function_disable_instance_layer(char * layer_name)
{
    uint32_t layer_idx;

    layer_idx = __function_find_instance_layer_idx(layer_name);
    if (layer_idx == function_ctx.instance_ctx.num_layers) {
        return FAILURE;
    }

    __function_set_layer_state(&function_ctx.instance_ctx, layer_idx,
                                        VULKAN_FUNCTION_STATE_DISABLED);
    return SUCCESS;
}

uint32_t function_get_num_instance_layers(void)
{
    return function_ctx.instance_ctx.num_layers;
}

uint32_t function_check_instance_layer_state(char *layer_name)
{
    uint32_t layer_idx;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;

    layer_idx = __function_find_instance_layer_idx(layer_name);
    if (layer_idx == p_ctx->num_layers) {
        return VULKAN_FUNCTION_STATE_INVALID;
    }

    return p_ctx->layer_ctx.states[layer_idx];
}

static uint32_t __function_get_matching_layer_info_count(function_setup_ctx_t *p_ctx,
                                                                        uint32_t state)
{
    uint32_t matching_entry_cnt;

    if (state == VULKAN_FUNCTION_STATE_DEFAULT) {
        return p_ctx->num_layers;
    }

    matching_entry_cnt = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        if (p_ctx->layer_ctx.states[idx] != state) {
            continue;
        }

        matching_entry_cnt++;
    }

    return matching_entry_cnt;
}

uint32_t function_get_instance_layers_count(uint32_t state)
{
    return __function_get_matching_layer_info_count(&function_ctx.instance_ctx, state);
}

uint32_t function_get_phydev_layers_count(uint32_t state, uint32_t phydev_idx)
{
    return __function_get_matching_layer_info_count(&function_ctx.p_phydevs_ctx[phydev_idx], state);
}

void function_get_instance_layers_name_list(uint32_t state, char **p_names_list)
{
    uint32_t list_idx;
    uint32_t list_cnt;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;

    list_cnt = function_get_instance_layers_count(state);

    char names_list[list_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    VkLayerProperties p_properties[p_ctx->num_layers];

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    list_idx = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        if ((state == p_ctx->layer_ctx.states[idx]) ||
            (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            strcpy(names_list[list_idx], p_properties[idx].layerName);
            list_idx++;
        }
    }

    memcpy(p_names_list, names_list, list_cnt * VK_MAX_EXTENSION_NAME_SIZE);

    return;
}

static uint32_t __function_find_extension_idx(VkExtensionProperties *p_list, uint32_t list_size,
                                                                        const char *extension_name)
{
    for (uint32_t extension_idx = 0; extension_idx < list_size; extension_idx++) {
        if (strcmp(extension_name, p_list[extension_idx].extensionName) == 0) {
            return extension_idx;
        }
    }

    return list_size;
}

static uint32_t __function_find_instance_extension_idx(char * extension_name)
{
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;

    VkExtensionProperties p_properties[p_ctx->num_extensions];

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, p_properties);

    return __function_find_extension_idx(p_properties, p_ctx->num_extensions, extension_name);
}

static uint32_t __function_set_instance_extension_state(char *extension_name, uint32_t state)
{
    uint32_t ext_idx;

    ext_idx = __function_find_instance_extension_idx(extension_name);
    if (ext_idx == function_ctx.instance_ctx.num_extensions) {
        return FAILURE;
    }

    function_ctx.instance_ctx.extension_ctx.states[ext_idx] = state;

    return SUCCESS;
}

static uint32_t __function_get_matching_extension_info_count(uint32_t state)
{
    uint32_t matching_entry_cnt;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;
    if (state == VULKAN_FUNCTION_STATE_DEFAULT) {
        return p_ctx->num_extensions;
    }

    matching_entry_cnt = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        if (p_ctx->extension_ctx.states[idx] != state) {
            continue;
        }

        matching_entry_cnt++;
    }

    return matching_entry_cnt;
}

uint32_t function_enable_instance_extension(char *extension_name)
{
    return __function_set_instance_extension_state(extension_name, VULKAN_FUNCTION_STATE_ENABLED);
}

uint32_t function_disable_instance_extension(char *extension_name)
{
    return __function_set_instance_extension_state(extension_name, VULKAN_FUNCTION_STATE_DISABLED);
}

uint32_t function_get_num_instance_extensions(void)
{
    return function_ctx.instance_ctx.num_extensions;
}

uint32_t function_check_instance_extension_state(char *extension_name)
{
    uint32_t extension_idx;

    extension_idx = __function_find_instance_extension_idx(extension_name);
    if (extension_idx == function_ctx.instance_ctx.num_extensions) {
        return VULKAN_FUNCTION_STATE_INVALID;
    }

    return function_ctx.instance_ctx.extension_ctx.states[extension_idx];
}

uint32_t function_get_instance_extensions_count(uint32_t state)
{
    return __function_get_matching_extension_info_count(state);
}

void function_get_instance_extensions_name_list(uint32_t state, char **p_names_list)
{
    uint32_t list_idx;
    uint32_t list_cnt;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.instance_ctx;

    list_cnt = function_get_instance_extensions_count(state);

    char names_list[list_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    VkExtensionProperties p_properties[p_ctx->num_extensions];

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, p_properties);

    list_idx = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        if ((state == p_ctx->extension_ctx.states[idx]) ||
                (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            strcpy(names_list[list_idx], p_properties[idx].extensionName);

            list_idx++;
        }
    }

    memcpy(p_names_list, names_list, list_cnt * VK_MAX_EXTENSION_NAME_SIZE);

    return;
}

static uint32_t __function_find_phydev_layer_idx(char *layer_name, uint32_t phydev_idx,
                                                                VkPhysicalDevice *p_phydev)
{
    uint32_t layer_idx;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    VkLayerProperties p_properties[p_ctx->num_layers];

    vkEnumerateDeviceLayerProperties(*p_phydev, &p_ctx->num_layers, p_properties);

    return __function_find_layer_idx(p_properties, p_ctx->num_layers, layer_name);
}

uint32_t function_enable_phydev_layer(char *layer_name, uint32_t phydev_idx,
                                                    VkPhysicalDevice *p_phydev)
{
    uint32_t layer_idx;
    layer_idx = __function_find_phydev_layer_idx(layer_name, phydev_idx, p_phydev);
    if (layer_idx == function_ctx.p_phydevs_ctx[phydev_idx].num_layers) {
        return FAILURE;
    }

    __function_set_layer_state(&function_ctx.p_phydevs_ctx[phydev_idx],
                                layer_idx, VULKAN_FUNCTION_STATE_ENABLED);
    return SUCCESS;
}

uint32_t function_disable_phydev_layer(char *layer_name, uint32_t phydev_idx,
                                                    VkPhysicalDevice *p_phydev)
{
    uint32_t layer_idx;

    layer_idx = __function_find_phydev_layer_idx(layer_name, phydev_idx, p_phydev);
    if (layer_idx == function_ctx.p_phydevs_ctx[phydev_idx].num_layers) {
        return FAILURE;
    }

    __function_set_layer_state(&function_ctx.p_phydevs_ctx[phydev_idx],
                                layer_idx, VULKAN_FUNCTION_STATE_DISABLED);
    return SUCCESS;
}

void function_get_phydev_layers_name_list(uint32_t state, char **p_names_list,
                                            uint32_t phydev_idx, VkPhysicalDevice *p_phydev)
{
    uint32_t list_idx;
    uint32_t list_cnt;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    list_cnt = function_get_phydev_layers_count(state, phydev_idx);

    char names_list[list_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    VkLayerProperties p_properties[p_ctx->num_layers];

    vkEnumerateDeviceLayerProperties(*p_phydev, &p_ctx->num_layers, p_properties);

    list_idx = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        if ((state == p_ctx->layer_ctx.states[idx]) ||
            (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            strcpy(names_list[list_idx], p_properties[idx].layerName);
            list_idx++;
        }
    }

    memcpy(p_names_list, names_list, list_idx * VK_MAX_EXTENSION_NAME_SIZE);

    return;
}

static uint32_t __function_find_phydev_ext_idx(char *extension_name, uint32_t phydev_idx,
                                                                VkPhysicalDevice *p_phydev)
{
    uint32_t ext_idx;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    VkExtensionProperties p_properties[p_ctx->num_extensions];

    vkEnumerateDeviceExtensionProperties(*p_phydev, NULL, &p_ctx->num_extensions, p_properties);

    return __function_find_extension_idx(p_properties, p_ctx->num_extensions, extension_name);
}

static uint32_t __function_set_phydev_extension_state(char *extension_name, uint32_t phydev_idx,
                                                        VkPhysicalDevice *p_phydev, uint32_t state)
{
    uint32_t ext_idx;
    function_setup_ctx_t *p_phydev_ctx;

    p_phydev_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    ext_idx = __function_find_phydev_ext_idx(extension_name, phydev_idx, p_phydev);
    if (ext_idx == p_phydev_ctx->num_extensions) {
        return FAILURE;
    }

    p_phydev_ctx->extension_ctx.states[ext_idx] = state;

    return SUCCESS;
}

uint32_t function_enable_phydev_extension(char *extension_name, uint32_t phydev_idx,
                                                            VkPhysicalDevice *p_phydev)
{
    return __function_set_phydev_extension_state(extension_name, phydev_idx,
                                                    p_phydev, VULKAN_FUNCTION_STATE_ENABLED);
}

uint32_t function_disable_phydev_extension(char *extension_name, uint32_t phydev_idx,
                                                            VkPhysicalDevice *p_phydev)
{
    return __function_set_phydev_extension_state(extension_name, phydev_idx,
                                                    p_phydev, VULKAN_FUNCTION_STATE_DISABLED);
}

void function_get_phydev_extensions_name_list(uint32_t state, char **p_names_list,
                                                uint32_t phydev_idx, VkPhysicalDevice *p_phydev)
{
    uint32_t list_cnt;
    uint32_t list_idx;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    list_cnt = function_get_phydev_extensions_count(state, phydev_idx);

    char names_list[list_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    VkExtensionProperties p_properties[p_ctx->num_extensions];

    vkEnumerateDeviceExtensionProperties(*p_phydev, NULL, &p_ctx->num_extensions, p_properties);

    list_idx = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        if ((state == p_ctx->extension_ctx.states[idx]) ||
                (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            strcpy(names_list[list_idx], p_properties[idx].extensionName);
            list_idx++;
        }
    }

    memcpy(p_names_list, names_list, list_cnt * VK_MAX_EXTENSION_NAME_SIZE);

    return;
}

uint32_t function_get_num_phydev_layers(void)
{
    return function_ctx.p_phydevs_ctx->num_layers;;
}

static uint32_t __function_get_matching_phydev_extension_info_count(uint32_t state,
                                                                    uint32_t phydev_idx)
{
    uint32_t matching_entry_cnt;
    function_setup_ctx_t *p_ctx;

    p_ctx = &function_ctx.p_phydevs_ctx[phydev_idx];

    if (state == VULKAN_FUNCTION_STATE_DEFAULT) {
        return p_ctx->num_extensions;
    }

    matching_entry_cnt = 0;
    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        if (function_ctx.p_phydevs_ctx[phydev_idx].extension_ctx.states[idx] != state) {
            continue;
        }

        matching_entry_cnt++;
    }

    return matching_entry_cnt;
}

uint32_t function_get_phydev_extensions_count(uint32_t state, uint32_t phydev_idx)
{
    return __function_get_matching_phydev_extension_info_count(state, phydev_idx);
}
