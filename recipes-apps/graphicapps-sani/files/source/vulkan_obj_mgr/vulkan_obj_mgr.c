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
    p_dev_list = device_create_physical_device_list(device_count);

    instance_setup_physical_device_lists(p_dev_list, device_count);

    device_register_physical_device_capabilities();
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

void vulkan_obj_mgr_exit(void)
{
    

}
