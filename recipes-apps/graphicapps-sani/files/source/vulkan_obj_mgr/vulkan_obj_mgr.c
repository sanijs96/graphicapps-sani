#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "vulkan_function.h"
#include "device/vulkan_device.h"
#include "instance/vulkan_instance.h"

#include "vulkan_obj_mgr.h"

// vulkan instance
void vulkan_obj_mgr_init(void)
{
    instance_init("Sani Vulkan Application");

    function_init();
}

uint32_t vulkan_obj_mgr_enable_layer(char * layer_name)
{
    return function_enable_layer(layer_name);
}

uint32_t vulkan_obj_mgr_disable_layer(char * layer_name)
{
    return function_disable_layer(layer_name);
}

void vulkan_obj_mgr_show_layers_list(void)
{
    uint32_t layer_cnt;
    uint32_t layer_status;

    layer_cnt = function_get_layers_count(VULKAN_FUNCTION_STATE_DEFAULT);

    char p_layers_name[layer_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    printf("[LAYER INFO]\n");

    layer_cnt = function_get_layers_count(VULKAN_FUNCTION_STATE_ENABLED);

    function_get_layers_name_list(VULKAN_FUNCTION_STATE_ENABLED, p_layers_name);
    for (uint32_t idx = 0; idx < layer_cnt; idx++) {
        printf("[%u] %s (%s)\n", idx, p_layers_name[idx], "EN");
    }

    layer_cnt = function_get_layers_count(VULKAN_FUNCTION_STATE_DISABLED);

    function_get_layers_name_list(VULKAN_FUNCTION_STATE_DISABLED, p_layers_name);
    for (uint32_t idx = 0; idx < layer_cnt; idx++) {
        printf("[%u] %s\n", idx, p_layers_name[idx]);
    }

    return;
}

uint32_t vulkan_obj_mgr_enable_extension(char *extension_name)
{
    return function_enable_extension(extension_name);
}

uint32_t vulkan_obj_mgr_disable_extension(char *extension_name)
{
    return function_disable_extension(extension_name);
}

void vulkan_obj_mgr_show_extensions_list(void)
{
    uint32_t ext_cnt;
    uint32_t ext_status;

    ext_cnt = function_get_extensions_count(VULKAN_FUNCTION_STATE_DEFAULT);

    char p_extensions_name[ext_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    printf("[EXTENSION INFO]\n");

    ext_cnt = function_get_extensions_count(VULKAN_FUNCTION_STATE_ENABLED);
    function_get_extensions_name_list(VULKAN_FUNCTION_STATE_ENABLED, p_extensions_name);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("[%u] %s (%s)\n", idx, p_extensions_name[idx], "EN");
    }

    ext_cnt = function_get_extensions_count(VULKAN_FUNCTION_STATE_DISABLED);
    function_get_extensions_name_list(VULKAN_FUNCTION_STATE_DISABLED, p_extensions_name);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("[%u] %s\n", idx, p_extensions_name[idx]);
    }

    return;
}

uint32_t vulkan_obj_mgr_enable_phydev_extension(char *extension_name, uint32_t phydev_idx)
{
    return function_enable_phydev_extension(extension_name, phydev_idx);
}

uint32_t vulkan_obj_mgr_disable_phydev_extension(char *extension_name, uint32_t phydev_idx)
{
    return function_disable_phydev_extension(extension_name, phydev_idx);
}

static void __vulkan_obj_mgr_init_device_ctx(void)
{
    uint32_t device_count;
    device_count = instance_get_physical_devices_count();

    VkPhysicalDevice *p_dev_list;
    p_dev_list = instance_create_physical_device_list(device_count);

    device_create_physical_device_ctx(device_count);

    for (uint32_t idx; idx < device_count; idx++) {
        device_register_physical_device(&p_dev_list[idx], idx);
        device_register_device_capability(idx);
    }

    function_init_phydev_extension_info();
}

uint32_t vulkan_obj_mgr_create_instance(void)
{
    uint32_t layers_cnt;
    uint32_t exts_cnt;

    layers_cnt = function_get_layers_count(VULKAN_FUNCTION_STATE_ENABLED);
    exts_cnt = function_get_extensions_count(VULKAN_FUNCTION_STATE_ENABLED);

    char p_layers_list[layers_cnt][VK_MAX_EXTENSION_NAME_SIZE];
    char p_exts_list[exts_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    function_get_layers_name_list(VULKAN_FUNCTION_STATE_ENABLED, p_layers_list);
    function_get_extensions_name_list(VULKAN_FUNCTION_STATE_ENABLED, p_exts_list);

    char *layers_list_ptr[layers_cnt];
    char *exts_list_ptr[exts_cnt];

    for (uint32_t idx = 0; idx < layers_cnt; idx++) {
        layers_list_ptr[idx] = p_layers_list[idx];
    }
    instance_add_layer_info(layers_cnt, layers_list_ptr);

    for (uint32_t idx = 0; idx < exts_cnt; idx++) {
        exts_list_ptr[idx] = p_exts_list[idx];
    }
    instance_add_extension_info(exts_cnt, exts_list_ptr);

    if (instance_create() != VK_SUCCESS) {
        return FAILURE;
    }

    __vulkan_obj_mgr_init_device_ctx();

    return SUCCESS;
}

VkInstance *vulkan_obj_mgr_get_instance_object(void)
{
    if (instance_check_creation_state() != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        return NULL;
    }

    return instance_get_instance_object();
}

uint32_t vulkan_obj_mgr_get_phydev_count(void)
{
    return device_get_phydevs_count();
}

uint32_t vulkan_obj_mgr_get_current_phydev_idx(void)
{
    return device_get_current_phydev_idx();
}

VkDevice *vulkan_obj_mgr_get_current_device_object(void)
{
    return device_get_current_device_object();
}

VkPhysicalDevice *vulkan_obj_mgr_get_phydev_object(uint32_t phydev_idx)
{
    return device_get_phydev_object(phydev_idx);
}

// TODO: customize queueus
uint32_t vulkan_obj_mgr_create_device(uint32_t phydev_idx)
{
    uint32_t exts_cnt;

    if (instance_check_creation_state() != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        return FAILURE;
    }

    exts_cnt = function_get_phydev_exts_count(VULKAN_FUNCTION_STATE_ENABLED, phydev_idx);

    char exts_name_list[exts_cnt][VK_MAX_EXTENSION_NAME_SIZE];
    function_get_phydev_exts_name_list(VULKAN_FUNCTION_STATE_ENABLED, exts_name_list, phydev_idx);

    char *name_list_ptr[exts_cnt];
    for (uint32_t idx = 0; idx < exts_cnt; idx++) {
        name_list_ptr[idx] = exts_name_list[idx];
    }

    device_add_enabled_exts_info(exts_cnt, name_list_ptr);

    if (device_create(phydev_idx) == VK_SUCCESS) {
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

static char *__vulkan_obj_mgr_get_device_type_string(uint32_t phydev_type)
{
    char * str;
    switch(phydev_type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            str = "other";
            break;

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            str = "gpu";
            break;

        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            str = "gpu(embed)";
            break;

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            str = "gpu(virtual)";
            break;

        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            str = "cpu";
            break;

        default:
            str = NULL;
            break;
    }

    return str;
}

static void __vulkan_obj_mgr_print_queue_flags(uint32_t queue_flag)
{
    if (queue_flag & VK_QUEUE_GRAPHICS_BIT)         printf("%s", "GRAPHICS");
    if (queue_flag & VK_QUEUE_COMPUTE_BIT)          printf("%s", ", COMPUTE");
    if (queue_flag & VK_QUEUE_TRANSFER_BIT)         printf("%s", ", TRANSFER");
    if (queue_flag & VK_QUEUE_SPARSE_BINDING_BIT)   printf("%s", ", SPARSE");
    if (queue_flag & VK_QUEUE_PROTECTED_BIT)        printf("%s", ", PROTECT");
    if (queue_flag & VK_QUEUE_VIDEO_DECODE_BIT_KHR) printf("%s", ", DECODE");
    if (queue_flag & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) printf("%s", ", ENCODE");
    if (queue_flag & VK_QUEUE_OPTICAL_FLOW_BIT_NV)  printf("%s", ", OPTFLOW");
}

void vulkan_obj_mgr_show_device_info(uint32_t phydev_idx)
{
    char *device_type_str;
    uint32_t device_cnt;
    uint32_t queue_pty_count;
    uint32_t current_phydev_idx;
    uint32_t current_device_status;
    VkQueueFamilyProperties *p_queue_pty;
    VkPhysicalDeviceProperties *p_pty;

    device_cnt = instance_get_physical_devices_count();

    p_pty = device_get_device_property(phydev_idx);

    queue_pty_count = device_get_device_queue_property_count(phydev_idx);

    p_queue_pty = device_get_device_queue_property(phydev_idx);

    device_type_str = __vulkan_obj_mgr_get_device_type_string(p_pty->deviceType);

    current_device_status = device_get_current_status();
    if (current_device_status == VULKAN_DEVICE_CREATION_STATE_CREATED) {
        current_phydev_idx = device_get_current_phydev_idx();
    }

    if ((phydev_idx == current_phydev_idx) &&
        (current_device_status == VULKAN_DEVICE_CREATION_STATE_CREATED)) {
        printf("*");
    }

    printf("[%u] %s\n", phydev_idx, p_pty->deviceName);

    printf("\t type: %s\n", device_type_str);

    printf("\t queues:\n");
    for (uint32_t queue_idx = 0; queue_idx < queue_pty_count; queue_idx++) {
        printf("\t\t[%u]: ", queue_idx);
        __vulkan_obj_mgr_print_queue_flags(p_queue_pty->queueFlags);
        printf("\n");

        printf("\t\tsupported queues: %u\n", p_queue_pty->queueCount);
        printf("\t\tgranularity: %u %u %u\n",
                p_queue_pty->minImageTransferGranularity.width,
                p_queue_pty->minImageTransferGranularity.height,
                p_queue_pty->minImageTransferGranularity.depth);
    }

    return;
}

void vulkan_obj_mgr_show_device_extensions_list(uint32_t phydev_idx)
{
    uint32_t ret;
    uint32_t ext_cnt;

    ext_cnt = function_get_phydev_exts_count(VULKAN_FUNCTION_STATE_DEFAULT, phydev_idx);

    char ext_list[ext_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    printf("[Extensions]\n");

    ext_cnt = function_get_phydev_exts_count(VULKAN_FUNCTION_STATE_ENABLED, phydev_idx);
    function_get_phydev_exts_name_list(VULKAN_FUNCTION_STATE_ENABLED, ext_list, phydev_idx);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("%u: %s (%s)\n", idx, ext_list[idx], "EN");
    }

    ext_cnt = function_get_phydev_exts_count(VULKAN_FUNCTION_STATE_DISABLED, phydev_idx);
    function_get_phydev_exts_name_list(VULKAN_FUNCTION_STATE_DISABLED, ext_list, phydev_idx);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("%u: %s\n", idx, ext_list[idx]);
    }
}

void vulkan_obj_mgr_exit(void)
{

}
