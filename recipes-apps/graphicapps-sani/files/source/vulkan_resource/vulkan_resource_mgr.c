#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common/common_def.h"

#include "handler/vulkan_resource_handlers.h"

#include "vulkan_resource_mgr.h"

enum resource_entry_state {
    RESOURCE_ENTRY_STATE_DEFAULT,
    RESOURCE_ENTRY_STATE_ALLOCATED,
    RESOURCE_ENTRY_STATE_MEMORY_BOUND,
};

enum memory_property_state {
    MEMOROY_PROPERTY_STATE_DEFAULT,
    MEMOROY_PROPERTY_STATE_REGISTERED,
};

typedef struct resource_entry {
    uint32_t state;

    void *p_host_resource_buf;

    resource_info_t info;

    union {
        VkImage image;
        VkBuffer buffer;
    } resource;

    uint32_t device_memory_idx;
    VkDeviceMemory device_memory;

    VkMemoryRequirements memory_requirement;

    struct resource_entry *p_next;
} resource_entry_t, *resource_list_t;

typedef struct device_memory_ctx {
    uint32_t state;

    VkPhysicalDeviceMemoryProperties property;
} device_memory_ctx_t;

static struct {
    resource_list_t buffer_list;
    resource_list_t image_list;
    resource_list_t vertex_list;

    device_memory_ctx_t device_memory_ctx;
} resource_ctx = { NULL, NULL, NULL, 0 };

uint32_t vulkan_resource_mgr_get_resource_data_unit_size(uint32_t type)
{
    uint32_t data_unit_size;

    switch (type) {
        case RESOURCE_FORMAT_TYPE_BUFFER_VERTEX_2D_RGB:
            data_unit_size = sizeof(vertex_2d_rgb_t);
            break;

        default:
            printf("no matching resource type %u\n", type);
            data_unit_size = 0;
    }

    return data_unit_size;
}

static resource_entry_t *__vulkan_resource_mgr_add_new_entry(resource_list_t *p_resource_list,
                                                                    resource_info_t *p_info)
{
    resource_entry_t *p_entry;

    p_entry = (resource_entry_t *)malloc(sizeof(resource_entry_t));

    p_entry->state = RESOURCE_ENTRY_STATE_DEFAULT;

    p_entry->p_next = *p_resource_list;

    *p_resource_list = p_entry;

    memcpy(&p_entry->info, p_info, sizeof(resource_info_t));

    p_entry->info.usage_flag = resource_handler_get_resource_usage_flag(p_info);

    return p_entry;
}

static void __vulkan_resource_mgr_delete_entry(resource_list_t p_list, resource_entry_t *p_entry)
{
    resource_entry_t *p_cursor;

    if (p_list == NULL) {
        return;
    }

    p_cursor = p_list;

    while (p_cursor->p_next != p_entry) {
        p_cursor = p_cursor->p_next;
    }

    free(p_entry);

    p_cursor->p_next = NULL;

    return;
}

uint32_t vulkan_resource_mgr_add_resource_info(resource_info_t *p_info)
{
    resource_entry_t *p_entry;

    if (p_info->type < MAX_RESOURCE_FORMAT_TYPE_VERTEX_BUFFERS) {
        p_entry = __vulkan_resource_mgr_add_new_entry(&resource_ctx.vertex_list, p_info);
    }
    else if (p_info->type < MAX_RESOURCE_FORMAT_TYPE_BUFFERS) {
        p_entry = __vulkan_resource_mgr_add_new_entry(&resource_ctx.buffer_list, p_info);
    }
    else {
        p_entry = __vulkan_resource_mgr_add_new_entry(&resource_ctx.image_list, p_info);
    }

    if (p_entry == NULL) {
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __vulkan_resource_mgr_create_buffer_object(VkDevice *p_device,
                                                            resource_entry_t *p_entry,
                                                                    void *p_resource_buf)
{
    uint32_t res;
    VkBufferCreateInfo info;

    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = NULL;
    info.flags = 0;

    // no sharing between queues
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = NULL;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // size may differ from device memory requirement size
    info.size = p_entry->info.count *
                    vulkan_resource_mgr_get_resource_data_unit_size(p_entry->info.type);
    info.usage = p_entry->info.usage_flag;

    res = vkCreateBuffer(*p_device, &info, NULL, p_resource_buf);
    if (res != VK_SUCCESS) {
        printf("create buffer failure: %d\n", res);

        return FAILURE;
    }

    return SUCCESS;
}

static resource_entry_t *__vulkan_resource_mgr_get_matching_resource_entry(char *resource_name,
                                                                            resource_list_t p_list)
{
    resource_entry_t *p_entry;

    p_entry = p_list;
    while (p_entry != NULL) {
        if (strcmp(p_entry->info.name, resource_name)) {
            p_entry = p_list->p_next;
        }

        break;
    }

    return p_entry;
}

resource_entry_t *vulkan_resource_mgr_get_resource_entry(char *resource_name)
{
    resource_entry_t *p_entry;

    p_entry = __vulkan_resource_mgr_get_matching_resource_entry(resource_name,
                                                                resource_ctx.vertex_list);
    if (p_entry != NULL) {
        return p_entry;
    }

    p_entry = __vulkan_resource_mgr_get_matching_resource_entry(resource_name,
                                                                resource_ctx.buffer_list);
    if (p_entry != NULL) {
        return p_entry;
    }

    p_entry = __vulkan_resource_mgr_get_matching_resource_entry(resource_name,
                                                                resource_ctx.image_list);
    if (p_entry != NULL) {
        return p_entry;
    }

    printf("no matching entry with %s\n", resource_name);

    return NULL;
}

resource_info_t *vulkan_resource_mgr_get_resource_info(char *resource_name)
{
    resource_entry_t *p_entry;

    p_entry = vulkan_resource_mgr_get_resource_entry(resource_name);
    if (p_entry != NULL) {
        return &p_entry->info;
    }

    printf("no matching entry with %s\n", resource_name);

    return NULL;
}

void *vulkan_resource_mgr_get_resource_object(char *resource_name)
{
    resource_entry_t *p_entry;

    p_entry = vulkan_resource_mgr_get_resource_entry(resource_name);
    if (p_entry == NULL) {
        return NULL;
    }

    return (void *)&p_entry->resource;
}

resource_description_t *vulkan_resource_mgr_get_resource_description(char *resource_name)
{
    resource_info_t *p_info;

    p_info = vulkan_resource_mgr_get_resource_info(resource_name);
    if (p_info == NULL) {
        return NULL;
    }

    return resource_handler_get_resource_description(p_info);
}

static uint32_t __vulkan_resource_mgr_get_resource_memory_requirement(VkDevice *p_device,
                                                                    resource_entry_t *p_entry)
{
    uint32_t requirement_flag;

    if (p_entry->info.type < MAX_RESOURCE_FORMAT_TYPE_BUFFERS) {
        vkGetBufferMemoryRequirements(*p_device, p_entry->resource.buffer,
                                                &p_entry->memory_requirement);
    }
    else {
    }

    requirement_flag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    return requirement_flag;
}

static uint32_t __vulkan_resource_mgr_get_device_memory_idx(uint32_t requirement_flag)
{
    VkPhysicalDeviceMemoryProperties *p_property;

    p_property = &resource_ctx.device_memory_ctx.property;

    for (uint32_t idx = 0; idx < p_property->memoryTypeCount; idx++) {
        if ((requirement_flag & p_property->memoryTypes[idx].propertyFlags) != requirement_flag) {
            continue;
        }

        return idx;
    }

    printf("no requirement satisfied with resource with requirement %x\n", requirement_flag);

    return FAILURE;
}

static uint32_t __vulkan_resource_mgr_allocate_device_memory(VkDevice *p_device,
                                                                resource_entry_t *p_entry)
{
    uint32_t res;
    uint32_t device_memory_idx;
    VkMemoryAllocateInfo info;

    if (p_entry == NULL) {
        return FAILURE;
    }

    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = NULL;

    info.allocationSize = p_entry->memory_requirement.size;
    info.memoryTypeIndex = p_entry->device_memory_idx;

    res = vkAllocateMemory(*p_device, &info, NULL, &p_entry->device_memory);
    if (res != VK_SUCCESS) {
        printf("memory allocation failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t vulkan_resource_mgr_create_vertex_buffer(VkDevice *p_device, char *resource_name,
                                                    resource_member_list_t *p_members_list)
{
    uint32_t requirement_flag;
    resource_entry_t *p_entry;
    resource_list_t resource_list;

    resource_list = resource_ctx.vertex_list;

    requirement_flag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    p_entry = __vulkan_resource_mgr_get_matching_resource_entry(resource_name, resource_list);
    if (p_entry == NULL) {
        return FAILURE;
    }

    if (__vulkan_resource_mgr_create_buffer_object(p_device, p_entry,
                                                    &p_entry->resource.buffer) == FAILURE) {
        return FAILURE;
    }

    vkGetBufferMemoryRequirements(*p_device, p_entry->resource.buffer,
                                            &p_entry->memory_requirement);

    p_entry->device_memory_idx = __vulkan_resource_mgr_get_device_memory_idx(requirement_flag);
    if (p_entry->device_memory_idx == resource_ctx.device_memory_ctx.property.memoryTypeCount) {
        return FAILURE;
    }

    if (__vulkan_resource_mgr_allocate_device_memory(p_device, p_entry) == FAILURE) {
        return FAILURE;
    }

    vkBindBufferMemory(*p_device, p_entry->resource.buffer, p_entry->device_memory, 0);

    vkMapMemory(*p_device, p_entry->device_memory, 0,
                p_entry->memory_requirement.size, 0, &p_entry->p_host_resource_buf);

    if (resource_handler_setup_resource_buf(p_entry->p_host_resource_buf,
                                            &p_entry->info, p_members_list) == FAILURE) {
        __vulkan_resource_mgr_delete_entry(resource_ctx.vertex_list, p_entry);
        return FAILURE;
    }

    return SUCCESS;
}

// TODO: delete?
uint32_t vulkan_resource_mgr_create_buffer(VkDevice *p_device, char *resource_name,
                                                    resource_member_list_t *p_members_list)
{
    uint32_t requirement_flag;
    resource_entry_t *p_entry;
    resource_list_t resource_list;

    resource_list = resource_ctx.buffer_list;

    requirement_flag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    p_entry = __vulkan_resource_mgr_get_matching_resource_entry(resource_name, resource_list);
    if (p_entry == NULL) {
        return FAILURE;
    }

    if (__vulkan_resource_mgr_create_buffer_object(p_device, p_entry,
                                                    &p_entry->resource.buffer) == FAILURE) {
        return FAILURE;
    }

    p_entry->device_memory_idx = __vulkan_resource_mgr_get_device_memory_idx(requirement_flag);
    if (p_entry->device_memory_idx == resource_ctx.device_memory_ctx.property.memoryTypeCount) {
        return FAILURE;
    }

    if (__vulkan_resource_mgr_allocate_device_memory(p_device, p_entry) == FAILURE) {
        return FAILURE;
    }

    vkBindBufferMemory(*p_device, p_entry->resource.buffer, p_entry->device_memory, 0);

    vkMapMemory(*p_device, p_entry->device_memory, 0,
                p_entry->memory_requirement.size, 0, &p_entry->p_host_resource_buf);

    if (resource_handler_setup_resource_buf(p_entry->p_host_resource_buf,
                                            &p_entry->info, p_members_list) == FAILURE) {
        __vulkan_resource_mgr_delete_entry(resource_ctx.vertex_list, p_entry);
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t vulkan_resource_mgr_create_image(VkDevice *p_device, char *resource_name,
                                                    resource_member_list_t *p_members_list)
{
        //vkGetImageMemoryRequirements(*p_device, p_entry->resource.image,
        //                                    &p_entry->memory_requirement);
}

uint32_t vulkan_resource_mgr_create_device_resource_copy(VkDevice *p_device, char *resource_name,
                                                                            void *p_resource_object)
{
    uint32_t res;
    uint32_t requirement_flag;
    resource_entry_t *p_entry, entry_copy;

    requirement_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    p_entry = vulkan_resource_mgr_get_resource_entry(resource_name);
    if (p_entry == NULL) {
        return FAILURE;
    }

    memcpy(&entry_copy, p_entry, sizeof(resource_entry_t));

    p_entry = &entry_copy;

    // TODO: check validity
    p_entry->info.usage_flag |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (p_entry->info.type < MAX_RESOURCE_FORMAT_TYPE_BUFFERS) {
        res = __vulkan_resource_mgr_create_buffer_object(p_device, p_entry, p_resource_object);
    }
    else { // TOOD: add image handler
        //res = __vulkan_resource_mgr_create_image_object(p_device, p_entry);
    }

    if (res == FAILURE) {
        return FAILURE;
    }

    p_entry->device_memory_idx = __vulkan_resource_mgr_get_device_memory_idx(requirement_flag);
    if (p_entry->device_memory_idx == resource_ctx.device_memory_ctx.property.memoryTypeCount) {
        return FAILURE;
    }

    if (__vulkan_resource_mgr_allocate_device_memory(p_device, p_entry) == FAILURE) {
        return FAILURE;
    }

    if (p_entry->info.type < MAX_RESOURCE_FORMAT_TYPE_BUFFERS) {
        res = vkBindBufferMemory(*p_device, *(VkBuffer *)p_resource_object, p_entry->device_memory, 0);
    }
    else { // TOOD: add image handler
        //res = __vulkan_resource_mgr_create_image_object(p_device, p_entry);
    }

    if (res != VK_SUCCESS) {
        printf("memory binding failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

void vulkan_resource_mgr_add_memory_property(VkPhysicalDevice *p_phydev)
{
    vkGetPhysicalDeviceMemoryProperties(*p_phydev, &resource_ctx.device_memory_ctx.property);

    resource_ctx.device_memory_ctx.state = MEMOROY_PROPERTY_STATE_REGISTERED;

    return;
}

void vulkan_resource_mgr_show_device_memory_properties(void)
{
    VkPhysicalDeviceMemoryProperties *p_property;

    if (resource_ctx.device_memory_ctx.state != MEMOROY_PROPERTY_STATE_REGISTERED) {
        printf("property not registered\n");
        return;
    }
    else {
        p_property = &resource_ctx.device_memory_ctx.property;
    }

    printf("Memory Types\n");

    printf("Heap Count %u\n", p_property->memoryHeapCount);

    for (uint32_t idx = 0; idx < p_property->memoryTypeCount; idx++) {
        printf("[%u]\n\ttypes 0x%x\n\theap idx %u\n",
                idx, p_property->memoryTypes[idx].propertyFlags,
                            p_property->memoryTypes[idx].heapIndex);
    }

    return;
}