#include <stdio.h>
#include <string.h>

#include "common/common_def.h"
#include "vulkan/queue_types.h"
#include "vulkan/function_scope.h"

#include "device/vulkan_device.h"
#include "instance/vulkan_instance.h"
#include "functions/vulkan_function.h"

#include "vulkan_obj_mgr.h"

// vulkan instance
void vulkan_obj_mgr_init(void)
{
    instance_init();

    function_init_layers_ctx();
}

uint32_t vulkan_obj_mgr_enable_layer(char * layer_name)
{
    return function_enable_instance_layer(layer_name);
}

uint32_t vulkan_obj_mgr_disable_layer(char * layer_name)
{
    return function_disable_instance_layer(layer_name);
}

void vulkan_obj_mgr_show_layers_list(void)
{
    uint32_t layer_cnt;
    uint32_t layer_state;

    layer_state = VULKAN_FUNCTION_STATE_DEFAULT;
    layer_cnt = function_get_instance_layers_count(layer_state);

    char p_layers_name[layer_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    printf("[LAYER INFO]\n");

    layer_cnt = function_get_instance_layers_count(VULKAN_FUNCTION_STATE_ENABLED);

    function_get_instance_layers_name_list(VULKAN_FUNCTION_STATE_ENABLED, (char **)p_layers_name);
    for (uint32_t idx = 0; idx < layer_cnt; idx++) {
        printf("[%u] %s (%s)\n", idx, p_layers_name[idx], "EN");
    }

    layer_cnt = function_get_instance_layers_count(VULKAN_FUNCTION_STATE_DISABLED);

    function_get_instance_layers_name_list(VULKAN_FUNCTION_STATE_DISABLED, (char **)p_layers_name);
    for (uint32_t idx = 0; idx < layer_cnt; idx++) {
        printf("[%u] %s\n", idx, p_layers_name[idx]);
    }

    return;
}

uint32_t vulkan_obj_mgr_enable_extension(char *extension_name)
{
    return function_enable_instance_extension(extension_name);
}

uint32_t vulkan_obj_mgr_disable_extension(char *extension_name)
{
    return function_disable_instance_extension(extension_name);
}

void vulkan_obj_mgr_show_extensions_list(void)
{
    uint32_t ext_cnt;
    uint32_t ext_status;

    ext_cnt = function_get_instance_extensions_count(VULKAN_FUNCTION_STATE_DEFAULT);

    char p_extensions_name[ext_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    printf("[EXTENSION INFO]\n");

    ext_cnt = function_get_instance_extensions_count(VULKAN_FUNCTION_STATE_ENABLED);
    function_get_instance_extensions_name_list(VULKAN_FUNCTION_STATE_ENABLED, (char **)p_extensions_name);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("[%u] %s (%s)\n", idx, p_extensions_name[idx], "EN");
    }

    ext_cnt = function_get_instance_extensions_count(VULKAN_FUNCTION_STATE_DISABLED);
    function_get_instance_extensions_name_list(VULKAN_FUNCTION_STATE_DISABLED, (char **)p_extensions_name);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("[%u] %s\n", idx, p_extensions_name[idx]);
    }

    return;
}

uint32_t vulkan_obj_mgr_enable_phydev_layer(char *layer_name, uint32_t phydev_idx)
{
    VkPhysicalDevice *p_phydev;
    p_phydev = device_get_phydev_object(phydev_idx);
    return function_enable_phydev_layer(layer_name, phydev_idx, p_phydev);
}

uint32_t vulkan_obj_mgr_disable_phydev_layer(char *layer_name, uint32_t phydev_idx)
{
    VkPhysicalDevice *p_phydev;
    p_phydev = device_get_phydev_object(phydev_idx);
    return function_disable_phydev_layer(layer_name, phydev_idx, p_phydev);
}

uint32_t vulkan_obj_mgr_enable_phydev_extension(char *extension_name, uint32_t phydev_idx)
{
    VkPhysicalDevice *p_phydev;
    p_phydev = device_get_phydev_object(phydev_idx);
    return function_enable_phydev_extension(extension_name, phydev_idx, p_phydev);
}

uint32_t vulkan_obj_mgr_disable_phydev_extension(char *extension_name, uint32_t phydev_idx)
{
    VkPhysicalDevice *p_phydev;
    p_phydev = device_get_phydev_object(phydev_idx);
    return function_disable_phydev_extension(extension_name, phydev_idx, p_phydev);
}

static inline void __vulkan_obj_mgr_init_device_ctx(void)
{
    uint32_t phydev_count;
    VkInstance *p_instance;

    p_instance = instance_get_instance_object();

    device_init_device_ctx(p_instance);

    phydev_count = device_get_phydevs_count();

    VkPhysicalDevice p_phydevs[phydev_count];
    for (uint32_t idx = 0; idx < phydev_count; idx++) {
        p_phydevs[idx] = *device_get_phydev_object(idx);
    }

    function_init_phydevs_ctx(phydev_count, p_phydevs);
}

uint32_t vulkan_obj_mgr_create_instance(void)
{
    uint32_t layers_cnt;
    uint32_t exts_cnt;

    layers_cnt = function_get_instance_layers_count(VULKAN_FUNCTION_STATE_ENABLED);

    char *layers_list_ptr[layers_cnt];
    char p_layers_list[layers_cnt][VK_MAX_EXTENSION_NAME_SIZE];
    if (layers_cnt > 0) {
        function_get_instance_layers_name_list(VULKAN_FUNCTION_STATE_ENABLED,
                                                        (char **)p_layers_list);

        for (uint32_t idx = 0; idx < layers_cnt; idx++) {
            layers_list_ptr[idx] = p_layers_list[idx];
        }

        instance_add_layer_info(layers_cnt, layers_list_ptr);
    }
    else {
        layers_list_ptr[0] = NULL;
    }

    exts_cnt = function_get_instance_extensions_count(VULKAN_FUNCTION_STATE_ENABLED);

    char *exts_list_ptr[exts_cnt];
    char p_exts_list[exts_cnt][VK_MAX_EXTENSION_NAME_SIZE];
    if (exts_cnt > 0) {
        function_get_instance_extensions_name_list(VULKAN_FUNCTION_STATE_ENABLED,
                                                            (char **)p_exts_list);

        for (uint32_t idx = 0; idx < exts_cnt; idx++) {
            exts_list_ptr[idx] = p_exts_list[idx];
        }

        instance_add_extension_info(exts_cnt, exts_list_ptr);
    }
    else {
        exts_list_ptr[0] = NULL;
    }

    if (instance_create(layers_list_ptr, exts_list_ptr) == FAILURE) {
        return FAILURE;
    }

    __vulkan_obj_mgr_init_device_ctx();

    return SUCCESS;
}

uint32_t vulkan_obj_mgr_delete_instance(void)
{
    instance_destroy();

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
    uint32_t layers_cnt;
    VkPhysicalDevice *p_phydev;

    if (instance_check_creation_state() != VULKAN_INSTANCE_CREATION_STATE_CREATED) {
        printf("instance already created\n");
        return FAILURE;
    }

    if (vulkan_obj_mgr_check_device_created() == TRUE) {
        printf("device already created\n");
        return FAILURE;
    }

    p_phydev = device_get_phydev_object(phydev_idx);

    layers_cnt = function_get_phydev_layers_count(VULKAN_FUNCTION_STATE_ENABLED, phydev_idx);
    exts_cnt = function_get_phydev_extensions_count(VULKAN_FUNCTION_STATE_ENABLED, phydev_idx);

    char *layers_list_ptr[layers_cnt];
    char *exts_list_ptr[exts_cnt];

    char layers_list[layers_cnt][VK_MAX_EXTENSION_NAME_SIZE];
    char exts_list[exts_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    function_get_phydev_layers_name_list(VULKAN_FUNCTION_STATE_ENABLED,
                                        (char **)layers_list, phydev_idx, p_phydev);
    function_get_phydev_extensions_name_list(VULKAN_FUNCTION_STATE_ENABLED,
                                        (char **)exts_list, phydev_idx, p_phydev);

    for (uint32_t idx = 0; idx < layers_cnt; idx++) {
        layers_list_ptr[idx] = layers_list[idx];
    }
    device_add_enabled_layers_list(layers_cnt, layers_list_ptr);

    for (uint32_t idx = 0; idx < exts_cnt; idx++) {
        exts_list_ptr[idx] = exts_list[idx];
    }
    device_add_enabled_extensions_list(exts_cnt, exts_list_ptr);

    return device_create(phydev_idx);
}

uint32_t vulkan_obj_mgr_check_device_created(void)
{
    if (device_get_current_status() == VULKAN_DEVICE_CREATION_STATE_CREATED) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

uint32_t vulkan_obj_mgr_add_wait_semaphore(VkSemaphore *p_semaphore, VkPipelineStageFlags stage)
{
    return device_add_wait_semaphore_ctx(p_semaphore, stage);
}

uint32_t vulkan_obj_mgr_add_signal_semaphore(VkSemaphore *p_semaphore)
{
    return device_add_signal_semaphore_ctx(p_semaphore);
}

uint32_t vulkan_obj_mgr_get_wait_semaphore_count(void)
{
    return device_get_wait_semaphore_count();
}

uint32_t vulkan_obj_mgr_get_signal_semaphore_count(void)
{
    return device_get_signal_semaphore_count();
}

VkPipelineStageFlags *vulkan_obj_mgr_get_wait_semaphore_stages(void)
{
    return device_get_wait_semaphore_stages();
}

VkSemaphore *vulkan_obj_mgr_get_wait_semaphore_objects(void)
{
    return device_get_wait_semaphore_objects();
}

VkSemaphore *vulkan_obj_mgr_get_signal_semaphore_objects(void)
{
    return device_get_signal_semaphore_objects();
}

uint32_t vulkan_obj_mgr_get_graphics_queue_family_idx(VkDevice *p_device)
{
    return device_get_queue_family_idx(VULKAN_DEVICE_QUEUE_TYPE_GRAPHICS);
}

uint32_t vulkan_obj_mgr_get_compute_queue_family_idx(VkDevice *p_device)
{
    return device_get_queue_family_idx(VULKAN_DEVICE_QUEUE_TYPE_COMPUTE);
}

uint32_t vulkan_obj_mgr_get_transfer_queue_family_idx(VkDevice *p_device)
{
    return device_get_queue_family_idx(VULKAN_DEVICE_QUEUE_TYPE_TRANSFER);
}

uint32_t vulkan_obj_mgr_select_available_queue_idx(uint32_t type)
{
    uint32_t queue_idx;
    VkQueue *p_target_queue;

    queue_idx = device_get_available_queue_idx(type);

    while (device_lock_queue(type, queue_idx) == FAILURE) {
        queue_idx = device_get_available_queue_idx(type);
    }

    return queue_idx;
}

VkQueue *vulkan_obj_mgr_get_queue_object(uint32_t type, uint32_t idx)
{
    device_get_queue_object(type, idx);
}

uint32_t vulkan_obj_mgr_submit_queue(VkQueue *p_queue, VkSubmitInfo *p_submit_info)
{
    if (device_submit_queue(p_queue, p_submit_info) == FAILURE) {
        printf("queue submission failure\n");
        return FAILURE;
    }

    return SUCCESS;
}

void vulkan_obj_mgr_release_queue(uint32_t type, uint32_t idx)
{
    device_unlock_queue(type, idx);
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

uint32_t vulkan_obj_mgr_get_device_queue_family_count(uint32_t phydev_idx)
{
    return device_get_device_queue_property_count(phydev_idx);
}

void vulkan_obj_mgr_show_device_info(uint32_t phydev_idx)
{
    char *device_type_str;
    uint32_t device_cnt;
    uint32_t queue_property_count;
    uint32_t current_phydev_idx;
    uint32_t current_device_status;
    VkPhysicalDeviceProperties phydev_property;

    device_cnt = device_get_phydevs_count();

    device_get_device_property(phydev_idx, &phydev_property);

    device_type_str = __vulkan_obj_mgr_get_device_type_string(phydev_property.deviceType);

    queue_property_count = device_get_device_queue_property_count(phydev_idx);

    VkQueueFamilyProperties queue_properties[queue_property_count];

    device_get_device_queue_properties(phydev_idx, queue_properties);

    current_device_status = device_get_current_status();
    if (current_device_status == VULKAN_DEVICE_CREATION_STATE_CREATED) {
        current_phydev_idx = device_get_current_phydev_idx();
    }

    if ((phydev_idx == current_phydev_idx) &&
        (current_device_status == VULKAN_DEVICE_CREATION_STATE_CREATED)) {
        printf("*");
    }

    printf("[%u] %s\n", phydev_idx, phydev_property.deviceName);

    printf("\t type: %s\n", device_type_str);

    printf("\t queues:\n");
    for (uint32_t queue_idx = 0; queue_idx < queue_property_count; queue_idx++) {
        printf("\t\t[%u]: ", queue_idx);
        __vulkan_obj_mgr_print_queue_flags(queue_properties[queue_idx].queueFlags);
        printf("\n");

        printf("\t\tsupported queues: %u\n", queue_properties[queue_idx].queueCount);
        printf("\t\tgranularity: %u %u %u\n",
                queue_properties[queue_idx].minImageTransferGranularity.width,
                queue_properties[queue_idx].minImageTransferGranularity.height,
                queue_properties[queue_idx].minImageTransferGranularity.depth);
    }

    return;
}

void vulkan_obj_mgr_show_device_extensions_list(uint32_t phydev_idx)
{
    uint32_t ret;
    uint32_t ext_cnt;
    VkPhysicalDevice *p_phydev;

    ext_cnt = function_get_phydev_extensions_count(VULKAN_FUNCTION_STATE_DEFAULT, phydev_idx);

    char ext_list[ext_cnt][VK_MAX_EXTENSION_NAME_SIZE];

    printf("[Extensions]\n");

    ext_cnt = function_get_phydev_extensions_count(VULKAN_FUNCTION_STATE_ENABLED, phydev_idx);
    p_phydev = device_get_phydev_object(phydev_idx);

    function_get_phydev_extensions_name_list(VULKAN_FUNCTION_STATE_ENABLED, (char **)ext_list,
                                                                            phydev_idx, p_phydev);

    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("%u: %s (%s)\n", idx, ext_list[idx], "EN");
    }

    ext_cnt = function_get_phydev_extensions_count(VULKAN_FUNCTION_STATE_DISABLED, phydev_idx);
    function_get_phydev_extensions_name_list(VULKAN_FUNCTION_STATE_DISABLED, (char **)ext_list,
                                                                            phydev_idx, p_phydev);
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        printf("%u: %s\n", idx, ext_list[idx]);
    }
}

void vulkan_obj_mgr_exit(void)
{

}
