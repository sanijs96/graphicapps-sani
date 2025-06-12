#include <stdio.h>
#include <string.h>

#include "common/common_def.h"

#include "device/vulkan_device.h"
#include "instance/vulkan_instance.h"

#include "vulkan_obj_mgr.h"

// vulkan instance
void vulkan_obj_mgr_init_instance_ctx(void)
{
    instance_init_ctx("Sani Vulkan Application");
}

uint32_t vulkan_obj_mgr_enable_layer(char * layer_name, uint32_t scope)
{
    if (scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        // TODO: implement device scope
        return FAILURE;
    }

    return instance_enable_layer(layer_name);
}

uint32_t vulkan_obj_mgr_disable_layer(char * layer_name, uint32_t scope)
{
    if (scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        // TODO: implement device scope
        return FAILURE;
    }

    return instance_disable_layer(layer_name);
}

uint32_t vulkan_obj_mgr_show_layers_list(void)
{
    uint32_t layer_cnt;
    uint32_t layer_status;
    VkLayerProperties *p_layer_property;

    layer_cnt = instance_get_num_layers();
    printf("[LAYER INFO]\n");
    for (uint32_t idx = 0; idx < layer_cnt; idx++) {
        p_layer_property = instance_get_layer_info(idx);
        layer_status = instance_check_layer_state(idx);

        printf("[%u] %s (%s)\n", idx, p_layer_property->layerName,
                (layer_status == VULKAN_INSTANCE_FUNCTION_STATE_ENABLED) ? "enabled" : "disabled");
        printf("\t%s\n", p_layer_property->description);
    }
}

uint32_t vulkan_obj_mgr_show_extensions_list(void)
{
    uint32_t ext_cnt;
    uint32_t ext_status;
    VkExtensionProperties *p_extension_property;

    ext_cnt = instance_get_num_extensions();

    printf("[EXTENSION INFO]\n");
    for (uint32_t idx = 0; idx < ext_cnt; idx++) {
        p_extension_property = instance_get_extension_info(idx);
        ext_status = instance_check_extension_state(idx);

        printf("[%u] %s (%s)\n", idx, p_extension_property->extensionName,
                (ext_status == VULKAN_INSTANCE_FUNCTION_STATE_ENABLED) ?  "enabled" : "disabled");
    }
}

uint32_t vulkan_obj_mgr_enable_extension(char *extension_name, uint32_t scope)
{
    if (scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        // TODO: implement device scope
        return FAILURE;
    }

    return instance_enable_extension(extension_name);
}

uint32_t vulkan_obj_mgr_disable_extension(char *extension_name, uint32_t scope)
{
    if (scope == VULKAN_FUNCTION_SCOPE_DEVICE) {
        // TODO: implement device scope
        return FAILURE;
    }

    return instance_disable_extension(extension_name);
}

uint32_t vulkan_obj_mgr_create_instance(void)
{
    if (instance_create() != VK_SUCCESS) {
        return FAILURE;
    }
    else {
        return SUCCESS;
    }
}

void vulkan_obj_mgr_init_device_ctx(void)
{
    uint32_t device_count;
    device_count = instance_get_physical_devices_count();

    VkPhysicalDevice *p_dev_list;
    p_dev_list = instance_create_physical_device_list(device_count);

    device_create_physical_device_ctx(device_count);

    for (uint32_t idx; idx < device_count; idx++) {
        device_register_physical_device_handler(&p_dev_list[idx], idx);

        device_register_physical_device_capability(idx);
    }
}

uint32_t vulkan_obj_mgr_create_device(uint32_t phydev_idx)
{
    // TODO: customize queueus
    device_queue_ctx_t queue_info = {
        .priorities = {VULKAN_DEVICE_QUEUE_PRIO_DEFAULT,
                        VULKAN_DEVICE_QUEUE_PRIO_DEFAULT,
                        VULKAN_DEVICE_QUEUE_PRIO_DEFAULT},
        .num_queues = 3
    };

    if (device_create(phydev_idx, queue_info) == VK_SUCCESS) {
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

static char *__vulkan_obj_mgr_get_queue_string(uint32_t queue_flag)
{
    char * str = NULL;

    if (queue_flag & VK_QUEUE_GRAPHICS_BIT)         strcat(str, "GRAPHICS");
    if (queue_flag & VK_QUEUE_COMPUTE_BIT)          strcat(str, ", COMPUTE");
    if (queue_flag & VK_QUEUE_TRANSFER_BIT)         strcat(str, ", TRANSFER");
    if (queue_flag & VK_QUEUE_SPARSE_BINDING_BIT)   strcat(str, ", SPARSE");
    if (queue_flag & VK_QUEUE_PROTECTED_BIT)        strcat(str, ", PROTECT");
    if (queue_flag & VK_QUEUE_VIDEO_DECODE_BIT_KHR) strcat(str, ", DECODE");
    if (queue_flag & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) strcat(str, ", ENCODE");
    if (queue_flag & VK_QUEUE_OPTICAL_FLOW_BIT_NV)  strcat(str, ", OPTFLOW");

    return str;
}

uint32_t vulkan_obj_mgr_show_devices_list(void)
{
    char *device_type_str;
    char *queue_flag_str;
    uint32_t device_cnt;
    dev_ctx_t device_ctx;
    uint32_t queue_pty_count;
    VkQueueFamilyProperties *p_queue_pty;
    VkPhysicalDeviceProperties *p_pty;

    device_cnt = instance_get_physical_devices_count();

    device_ctx = device_get_current_device_info();

    printf("[DEVICE LIST]\n");
    for (uint32_t idx = 0; idx < device_cnt; idx++) {
        p_pty = device_get_device_property(idx);

        queue_pty_count = device_get_num_device_queue_properties(idx);
        p_queue_pty = device_get_device_queue_property(idx);

        device_type_str = __vulkan_obj_mgr_get_device_type_string(p_pty->deviceType);

        if ((idx == device_ctx.phydev_idx) &&
            (device_ctx.status == VULKAN_DEVICE_CREATION_STATE_CREATED)) {
            printf("*");
        }

        printf("[%u] %s\n", idx, p_pty->deviceName);

        printf("\t type: %s\n", device_type_str);

        printf("\t queues:\n");
        for (uint32_t queue_idx = 0; queue_idx < queue_pty_count; queue_idx++) {
            queue_flag_str = __vulkan_obj_mgr_get_queue_string(p_queue_pty->queueFlags);
            printf("\t\t[%u] %s\n", queue_idx, queue_flag_str);
        }
    }

    return SUCCESS;
}

void vulkan_obj_mgr_exit(void)
{
    

}
