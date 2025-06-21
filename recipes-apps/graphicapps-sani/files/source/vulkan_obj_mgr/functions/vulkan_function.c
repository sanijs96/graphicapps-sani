#include <malloc.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_function.h"

typedef struct extension_ctx {
    uint32_t state;
    const VkExtensionProperties *p_pty;
} extension_ctx_t;

typedef struct layer_ctx {
    uint32_t state;
    uint32_t num_extensions;
    const VkLayerProperties *p_pty;
} layer_ctx_t;

struct function_ctx_t {
    uint32_t num_layers;
    uint32_t num_extensions;

    layer_ctx_t *p_layers;
    extension_ctx_t *p_extensions;
} function_ctx;

static void __function_init_extensions_ctx(void)
{
    struct function_ctx_t *p_ctx;
    extension_ctx_t *p_extensions;
    VkExtensionProperties *p_properties;

    p_ctx = &function_ctx;

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, NULL);

    p_properties = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) *
                                                                p_ctx->num_extensions);
    p_extensions = (extension_ctx_t *)malloc(sizeof(extension_ctx_t) * p_ctx->num_extensions);

    vkEnumerateInstanceExtensionProperties(NULL, &p_ctx->num_extensions, p_properties);

    for (uint32_t idx = 0; idx < p_ctx->num_extensions; idx++) {
        p_extensions[idx].p_pty = &p_properties[idx];
        p_extensions[idx].state = VULKAN_FUNCTION_STATE_DISABLED;
    }

    p_ctx->p_extensions = p_extensions;
}

static void __function_init_layers_ctx(void)
{
    struct function_ctx_t *p_ctx;
    VkLayerProperties *p_properties;
    layer_ctx_t *p_layers;

    p_ctx = &function_ctx;

    p_ctx->num_layers = 0;
    p_properties = NULL;

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    p_properties = (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * p_ctx->num_layers);
    p_layers = (layer_ctx_t *)malloc(sizeof(layer_ctx_t) * p_ctx->num_layers);

    vkEnumerateInstanceLayerProperties(&p_ctx->num_layers, p_properties);

    for (uint32_t idx = 0; idx < p_ctx->num_layers; idx++) {
        p_layers[idx].p_pty = &p_properties[idx];
        p_layers[idx].state = VULKAN_FUNCTION_STATE_DISABLED;
    }

    p_ctx->p_layers = p_layers;
}

void function_init(void)
{
    __function_init_layers_ctx();

    __function_init_extensions_ctx();
}

static uint32_t __function_find_layer_idx(const char * layer_name)
{
    uint32_t layer_idx;
    layer_ctx_t *p_layer_ctx;

    p_layer_ctx = &function_ctx.p_layers[0];

    layer_idx = 0;
    for (; layer_idx < function_ctx.num_layers; layer_idx++) {
        if (strcmp(layer_name, p_layer_ctx->p_pty->layerName) == 0) {
            break;
        }

        p_layer_ctx++;
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

uint32_t function_enable_layer(char * layer_name, uint32_t scope)
{
    uint32_t state;

    if (scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        state = VULKAN_FUNCTION_STATE_ENABLED_SCOPE_INSTANCE;
    }
    else if (scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        state = VULKAN_FUNCTION_STATE_ENABLED_SCOPE_DEVICE;
    }

    return __function_set_layer_state(layer_name, state);
}

uint32_t function_disable_layer(char * layer_name, uint32_t scope)
{
    uint32_t layer_idx;
    uint32_t function_state;

    layer_idx = __function_find_layer_idx(layer_name);
    function_state = function_ctx.p_extensions[layer_idx].state;

    if (scope == VULKAN_FUNCTION_SCOPE_DEFAULT) {
        return __function_set_layer_state(layer_name, VULKAN_FUNCTION_STATE_DISABLED);
    }

    if ((scope == VULKAN_FUNCTION_SCOPE_INSTANCE) &&
        (function_state == VULKAN_FUNCTION_STATE_ENABLED_SCOPE_INSTANCE)) {
        return __function_set_layer_state(layer_name, VULKAN_FUNCTION_STATE_DISABLED);
    }

    if ((scope == VULKAN_FUNCTION_SCOPE_DEVICE) &&
        (function_state == VULKAN_FUNCTION_STATE_ENABLED_SCOPE_DEVICE)) {
        return __function_set_layer_state(layer_name, VULKAN_FUNCTION_STATE_DISABLED);
    }

    return FAILURE;
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

void function_get_layer_names_list(uint32_t state, char **names_list)
{
    uint32_t list_entry_cnt;
    layer_ctx_t *p_ctx;

    list_entry_cnt = 0;
    p_ctx = function_ctx.p_layers;

    for (uint32_t idx = 0; idx < function_ctx.num_layers; idx++) {
        if ((state == p_ctx->state) || (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            names_list[list_entry_cnt] = p_ctx->p_pty->layerName;

            list_entry_cnt++;
        }

        p_ctx++;
    }
}

static uint32_t __function_find_extension_idx(char * extension_name)
{
    extension_ctx_t *p_extension_ctx;
    uint32_t ext_idx;
    ext_idx = 0;

    p_extension_ctx = function_ctx.p_extensions;

    for (;ext_idx < function_ctx.num_extensions; ext_idx++) {
        if (!strcmp(extension_name, p_extension_ctx->p_pty->extensionName)) {
            break;
        }
        p_extension_ctx++;
    }

    return ext_idx;
}

static uint32_t __function_get_extension_state(char * extension_name)
{
    uint32_t ext_idx;
    ext_idx = __function_find_extension_idx(extension_name);

    return function_ctx.p_extensions[ext_idx].state;
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

uint32_t function_enable_extension(char *extension_name, uint32_t scope)
{
    uint32_t state;

    if (scope == VULKAN_FUNCTION_SCOPE_INSTANCE) {
        state = VULKAN_FUNCTION_STATE_ENABLED_SCOPE_INSTANCE;
    }
    else if (scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        state = VULKAN_FUNCTION_STATE_ENABLED_SCOPE_DEVICE;
    }

    return __function_set_extension_state(extension_name, state);
}

uint32_t function_disable_extension(char *extension_name, uint32_t scope)
{
    uint32_t ext_idx;
    uint32_t function_state;

    ext_idx = __function_find_extension_idx(extension_name);
    function_state = function_ctx.p_extensions[ext_idx].state;

    if (scope == VULKAN_FUNCTION_SCOPE_DEFAULT) {
        return __function_set_extension_state(extension_name, VULKAN_FUNCTION_STATE_DISABLED);
    }

    if ((scope == VULKAN_FUNCTION_SCOPE_INSTANCE) &&
        (function_state == VULKAN_FUNCTION_STATE_ENABLED_SCOPE_INSTANCE)) {
        return __function_set_extension_state(extension_name, VULKAN_FUNCTION_STATE_DISABLED);
    }

    if ((scope == VULKAN_FUNCTION_SCOPE_DEVICE) &&
        (function_state == VULKAN_FUNCTION_STATE_ENABLED_SCOPE_DEVICE)) {
        return __function_set_extension_state(extension_name, VULKAN_FUNCTION_STATE_DISABLED);
    }

    return FAILURE;
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

uint32_t function_get_extensions_count(uint32_t state)
{
    return __function_get_matching_extension_info_count(state);
}

void function_get_extension_names_list(uint32_t state, char **names_list)
{
    uint32_t list_entry_cnt;
    extension_ctx_t *p_ctx;

    list_entry_cnt = 0;
    p_ctx = function_ctx.p_extensions;

    for (uint32_t idx = 0; idx < function_ctx.num_extensions; idx++) {
        if ((state == p_ctx->state) || (state == VULKAN_FUNCTION_STATE_DEFAULT)) {
            names_list[list_entry_cnt] = p_ctx->p_pty->extensionName;

            list_entry_cnt++;
        }

        p_ctx++;
    }
}
