#include <stdio.h>
#include <malloc.h>

#include "common/common_def.h"
#include "window_obj_mgr.h"

#if defined(GLFW_INCLUDE_VULKAN)
#include "glfw/window_glfw.h"
#endif

typedef struct swapchain_ctx {
    VkSwapchainKHR swapchain;

    uint32_t image_count;
    uint32_t current_image_idx;
    VkImage *p_images;
    VkImageView *p_views;

    VkFence image_fence;
    VkSemaphore render_finish_semaphore;
    VkSemaphore image_available_semaphore;
} swapchain_ctx_t;

typedef struct framebuffer_ctx {
    uint32_t framebuffer_count;
    VkFramebuffer *p_framebufs;
} framebuffer_ctx_t;

typedef struct display_ctx {
    uint32_t state;

    VkSurfaceKHR surface;

    uint32_t format_cnt;
    VkSurfaceFormatKHR base_format;
    VkExtent2D base_extent;

    uint32_t present_mode_cnt;
    VkSurfaceCapabilitiesKHR capability;

    swapchain_ctx_t swapchain_ctx;

    framebuffer_ctx_t framebuffer_ctx;
} display_ctx_t;

typedef struct window_ops {
    uint32_t (* create)(void);
    void (* resize)(uint32_t width, uint32_t height);
    uint32_t (* display)(VkSurfaceKHR *p_surface, VkInstance *p_instance);
    void (* exit)(VkSurfaceKHR *p_surface, VkInstance *p_instance);
} window_ops_t;

static struct {
    uint32_t type;
    window_ops_t ops;

    VkDevice *p_device;
    VkPhysicalDevice *p_phydev;

    display_ctx_t display_ctx;
} window_ctx;

uint32_t window_obj_mgr_init(uint32_t window_types)
{
    if (window_types == WINDOW_OBJ_TYPE_NONE) {
        return FAILURE;
    }

#if defined(GLFW_INCLUDE_VULKAN)
    glfw_init();
    window_ctx.type = WINDOW_OBJ_TYPE_GLFW;
    window_ctx.ops.create = glfw_create_window;
    window_ctx.ops.resize = glfw_change_window_size;
    window_ctx.ops.display = glfw_display_window;
    window_ctx.ops.exit = glfw_destroy_window;
#endif

    if (!window_ctx.ops.create) {
        return FAILURE;
    }

    window_ctx.ops.create();

    window_ctx.display_ctx.state = WINDOW_OBJ_DISPLAY_STATE_DEFAULT;

    if (window_ctx.ops.resize) {
        window_ctx.ops.resize(DEFAULT_WINDOW_SIZE_WIDTH, DEFAULT_WINDOW_SIZE_HEIGHT);
    }

    return SUCCESS;
}

uint32_t window_obj_mgr_resize(uint32_t width, uint32_t height)
{
    window_ctx.ops.resize(width, height);

    return SUCCESS;
}

static VkSurfaceFormatKHR __window_obj_mgr_select_base_format(display_ctx_t *p_display_ctx)
{
    uint32_t format_idx;
    VkSurfaceFormatKHR formats[p_display_ctx->format_cnt];

    vkGetPhysicalDeviceSurfaceFormatsKHR(*window_ctx.p_phydev, p_display_ctx->surface,
                                                        &p_display_ctx->format_cnt, formats);

    format_idx = 0;
    for (; format_idx < p_display_ctx->format_cnt; format_idx++) {
        if ((formats[format_idx].format == VK_FORMAT_B8G8R8A8_SRGB) &&
            (formats[format_idx].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
            break;
        }
    }

    return formats[format_idx];
}

static uint32_t __window_obj_mgr_setup_display_ctx(void)
{
    display_ctx_t *p_display_ctx;

    p_display_ctx = &window_ctx.display_ctx;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*window_ctx.p_phydev, p_display_ctx->surface,
                                                                    &p_display_ctx->capability);
    p_display_ctx->base_extent = p_display_ctx->capability.currentExtent;

    vkGetPhysicalDeviceSurfaceFormatsKHR(*window_ctx.p_phydev, p_display_ctx->surface,
                                                            &p_display_ctx->format_cnt, NULL);
    p_display_ctx->base_format = __window_obj_mgr_select_base_format(p_display_ctx);

    vkGetPhysicalDeviceSurfacePresentModesKHR(*window_ctx.p_phydev, p_display_ctx->surface,
                                                        &p_display_ctx->present_mode_cnt, NULL);
}

static uint32_t __window_obj_mgr_create_swapchain(void)
{
    display_ctx_t *p_display_ctx;
    VkSwapchainCreateInfoKHR create_info;

    p_display_ctx = &window_ctx.display_ctx;

    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = NULL;
    create_info.surface = p_display_ctx->surface;

    create_info.flags = 0;

    create_info.minImageCount = p_display_ctx->capability.minImageCount;

    create_info.imageFormat = p_display_ctx->base_format.format;
    create_info.imageColorSpace = p_display_ctx->base_format.colorSpace;
    create_info.imageExtent = p_display_ctx->capability.currentExtent;

    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;

    create_info.preTransform = p_display_ctx->capability.currentTransform;

    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    return vkCreateSwapchainKHR(*window_ctx.p_device, &create_info, NULL,
                                        &p_display_ctx->swapchain_ctx.swapchain);
}

static uint32_t __window_obj_mgr_create_swapchain_image_views(display_ctx_t *p_display_ctx)
{
    uint32_t res;
    swapchain_ctx_t *p_swapchain_ctx;
    VkImageViewCreateInfo create_info;

    p_swapchain_ctx = &p_display_ctx->swapchain_ctx;

    p_swapchain_ctx->p_views = (VkImageView *)malloc(sizeof(VkImageView) *
                                                        p_swapchain_ctx->image_count);

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.pNext = NULL;
    create_info.flags = 0;

    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

    create_info.format = p_display_ctx->base_format.format;

    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;

    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    for (uint32_t idx = 0; idx < p_swapchain_ctx->image_count; idx++) {
        create_info.image = p_swapchain_ctx->p_images[idx];

        res = vkCreateImageView(*window_ctx.p_device, &create_info,
                                NULL, &p_swapchain_ctx->p_views[idx]);
        if (res != VK_SUCCESS) {
            printf("image view creation failure: %d\n", res);

            return FAILURE;
        }
    }

    return SUCCESS;
}

static uint32_t __window_obj_mgr_create_swapchain_sync_objects(void)
{
    uint32_t res;
    VkFenceCreateInfo fence_info;
    VkSemaphoreCreateInfo semaphore_info;

    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // create with signaled state
    fence_info.pNext = NULL;

    res = vkCreateFence(*window_ctx.p_device, &fence_info, NULL,
                            &window_ctx.display_ctx.swapchain_ctx.image_fence);
    if (res != VK_SUCCESS) {
        printf("fence creation failure: %d\n", res);

        return FAILURE;
    }

    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = 0;
    semaphore_info.pNext = NULL;

    res = vkCreateSemaphore(*window_ctx.p_device, &semaphore_info, NULL,
                            &window_ctx.display_ctx.swapchain_ctx.image_available_semaphore);
    if (res != VK_SUCCESS) {
        printf("semaphore create failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

static uint32_t __window_obj_mgr_setup_swapchain_ctx(display_ctx_t *p_display_ctx)
{
    uint32_t res;
    swapchain_ctx_t *p_swapchain_ctx;

    p_swapchain_ctx = &p_display_ctx->swapchain_ctx;

    vkGetSwapchainImagesKHR(*window_ctx.p_device, p_swapchain_ctx->swapchain,
                                                &p_swapchain_ctx->image_count, NULL);
    p_swapchain_ctx->p_images = (VkImage *)malloc(sizeof(VkImage) * p_swapchain_ctx->image_count);
    vkGetSwapchainImagesKHR(*window_ctx.p_device, p_swapchain_ctx->swapchain,
                                        &p_swapchain_ctx->image_count, p_swapchain_ctx->p_images);

    res = __window_obj_mgr_create_swapchain_image_views(p_display_ctx);
    if (res == FAILURE) {
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t window_obj_mgr_setup_display(VkInstance *p_instance)
{
    uint32_t res;
    display_ctx_t *p_display_ctx;

    p_display_ctx = &window_ctx.display_ctx;

    if (window_ctx.ops.display(&p_display_ctx->surface, p_instance) != SUCCESS) {
        return FAILURE;
    }

    __window_obj_mgr_setup_display_ctx();

    res = __window_obj_mgr_create_swapchain();
    if (res != VK_SUCCESS) {
        printf("swapchain creation failure: %d\n", res);
        return FAILURE;
    }

    p_display_ctx->state = WINDOW_OBJ_DISPLAY_STATE_CREATED;

    res = __window_obj_mgr_setup_swapchain_ctx(p_display_ctx);
    if (res != VK_SUCCESS) {
        printf("swapchain ctx setup error: %d\n", res);
        return FAILURE;
    }

    return res;
}

static uint32_t __window_obj_mgr_get_next_image(VkDevice *p_device)
{
    uint32_t image_idx;
    VkSemaphore *p_semaphore;
    VkSwapchainKHR *p_swapchain;

    p_semaphore = &window_ctx.display_ctx.swapchain_ctx.image_available_semaphore;
    p_swapchain = &window_ctx.display_ctx.swapchain_ctx.swapchain;

    vkAcquireNextImageKHR(*p_device, *p_swapchain, UINT64_MAX,
                            *p_semaphore, VK_NULL_HANDLE, &image_idx);

    return image_idx;
}

uint32_t window_obj_mgr_start_display(VkDevice *p_device)
{
    uint32_t wait_all;
    swapchain_ctx_t *p_swapchain_ctx;
    VkFence *p_image_fence;

    wait_all = VK_TRUE;
    p_swapchain_ctx = &window_ctx.display_ctx.swapchain_ctx;
    p_image_fence = &p_swapchain_ctx->image_fence;

    if (window_ctx.display_ctx.state == WINDOW_OBJ_DISPLAY_STATE_CREATED) {
        if (__window_obj_mgr_create_swapchain_sync_objects() == FAILURE) {
            return FAILURE;
        }

        window_ctx.display_ctx.state = WINDOW_OBJ_DISPLAY_STATE_STARTED;
    }
    else if (window_ctx.display_ctx.state == WINDOW_OBJ_DISPLAY_STATE_STARTED) {
        vkWaitForFences(*p_device, 1, p_image_fence, wait_all, UINT64_MAX);
        vkResetFences(*p_device, 1, p_image_fence);
    }
    else {
        printf("create window object first\n");
        return FAILURE;
    }

    p_swapchain_ctx->current_image_idx = __window_obj_mgr_get_next_image(p_device);

    return SUCCESS;
}

uint32_t window_obj_mgr_get_next_framebuffer_image_idx(void)
{
    return window_ctx.display_ctx.swapchain_ctx.current_image_idx;
}

static uint32_t __window_obj_mgr_start_presentation(VkQueue *p_queue)
{
    uint32_t res;
    swapchain_ctx_t *p_swapchain_ctx;
    VkPresentInfoKHR present_info;

    p_swapchain_ctx = &window_ctx.display_ctx.swapchain_ctx;

    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = NULL;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &p_swapchain_ctx->render_finish_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &p_swapchain_ctx->swapchain;
    present_info.pImageIndices = &p_swapchain_ctx->current_image_idx;

    present_info.pResults = NULL;

    res = vkQueuePresentKHR(*p_queue, &present_info);
    if (res != VK_SUCCESS) {
        printf("presentation failure: %d\n", res);
        return FAILURE;
    }

    return SUCCESS;
}

uint32_t window_obj_mgr_show_queue_result(VkQueue *p_queue)
{
    return __window_obj_mgr_start_presentation(p_queue);
}

VkSemaphore *window_obj_mgr_get_display_semaphore_object(void)
{
    return &window_ctx.display_ctx.swapchain_ctx.image_available_semaphore;
}

void window_obj_mgr_add_swapchain_signal_semaphore(VkSemaphore *p_semaphore)
{
    window_ctx.display_ctx.swapchain_ctx.render_finish_semaphore = *p_semaphore;

}

uint32_t window_obj_mgr_check_display_status(void)
{
    return window_ctx.display_ctx.state;
}

void window_obj_mgr_setup_device_ctx(VkPhysicalDevice *p_phydev, VkDevice *p_device)
{
    window_ctx.p_phydev = p_phydev;
    window_ctx.p_device = p_device;
}

static void __window_obj_mgr_show_display_capabilities(VkSurfaceCapabilitiesKHR *p_cap)
{
    printf("\nCapabilities\n");
    printf("\tminImageCount       : %u\n", p_cap->minImageCount);
    printf("\tmaxImageCount       : %u\n", p_cap->maxImageCount);

    printf("\tcurrentExtent       : %u, %u\n",
            p_cap->currentExtent.width, p_cap->currentExtent.height);
    printf("\tminImageExtent      : %u, %u\n",
            p_cap->minImageExtent.width, p_cap->minImageExtent.height);
    printf("\tmaxImageExtent      : %u, %u\n",
            p_cap->maxImageExtent.width, p_cap->maxImageExtent.height);

    printf("\tmaxImageArrayLayers : %u\n", p_cap->maxImageArrayLayers);

    printf("\tsupportedTransforms : 0x%x\n", p_cap->supportedTransforms);
    printf("\tcurrentTransform    : 0x%x\n", p_cap->currentTransform);
    printf("\tsupportedCompositeA : 0x%x\n", p_cap->supportedCompositeAlpha);
    printf("\tsupportedUsageFlags : 0x%x\n", p_cap->supportedUsageFlags);
}

static void __window_obj_mgr_show_display_present_modes(VkPresentModeKHR *p_modes, uint32_t cnt)
{
    const char *present_modes[4] = {
        [VK_PRESENT_MODE_IMMEDIATE_KHR] = "IMMEDIATE",
        [VK_PRESENT_MODE_MAILBOX_KHR] = "MAILBOX",
        [VK_PRESENT_MODE_FIFO_KHR] = "FIFO",
        [VK_PRESENT_MODE_FIFO_RELAXED_KHR] = "FIFO_RELAX"
    };

    printf("\nPresentation Modes\n");
    for (uint32_t idx = 0; idx < cnt; idx++) {
        printf("\t%s: supported\n", present_modes[p_modes[idx]]);
    }
}

uint32_t window_obj_mgr_show_display_ctx_info(VkPhysicalDevice *p_phydev)
{
    uint32_t format_cnt;
    uint32_t present_cnt;
    display_ctx_t *p_display_ctx;

    format_cnt = window_ctx.display_ctx.format_cnt;
    present_cnt = window_ctx.display_ctx.present_mode_cnt;
    p_display_ctx = &window_ctx.display_ctx;

    __window_obj_mgr_show_display_capabilities(&p_display_ctx->capability);

    VkSurfaceFormatKHR formats[format_cnt];
    vkGetPhysicalDeviceSurfaceFormatsKHR(*p_phydev, p_display_ctx->surface, &format_cnt, formats);

    printf("\nFormats\n");
    for (uint32_t idx = 0; idx < format_cnt; idx++) {
        if ((formats[idx].format == VK_FORMAT_B8G8R8A8_SRGB) &&
            (formats[idx].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
            printf("\t [%u] Format VK_FORMAT_B8G8R8A8_SRGB: supported\n", idx);
            printf("\t [%u] Colorspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: supported\n", idx);
            break;
        }
    }

    VkPresentModeKHR present_modes[present_cnt];
    vkGetPhysicalDeviceSurfacePresentModesKHR(*p_phydev, p_display_ctx->surface,
                                                    &present_cnt, present_modes);
    __window_obj_mgr_show_display_present_modes(present_modes, present_cnt);

    return SUCCESS;
}

VkSurfaceKHR *window_obj_mgr_get_current_display_object(void)
{
    return &window_ctx.display_ctx.surface;
}

VkSwapchainKHR *window_obj_mgr_get_current_swapchain_object(void)
{
    return &window_ctx.display_ctx.swapchain_ctx.swapchain;
}

VkExtent2D *window_obj_mgr_get_current_swapchain_extent(void)
{
    return &window_ctx.display_ctx.base_extent;
}

VkFormat *window_obj_mgr_get_current_swapchain_format(void)
{
    return &window_ctx.display_ctx.base_format.format;
}

uint32_t window_obj_mgr_create_framebuffers(VkRenderPass* p_renderpass)
{
    uint32_t res;
    uint32_t image_count;
    VkFramebuffer *p_framebuffers;
    VkFramebufferCreateInfo framebuffer_info;

    image_count = window_ctx.display_ctx.swapchain_ctx.image_count;

    p_framebuffers = (VkFramebuffer *)malloc(sizeof(VkFramebuffer[image_count]));

    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.pNext = NULL;
    framebuffer_info.flags = 0;

    framebuffer_info.renderPass = *p_renderpass;

    framebuffer_info.attachmentCount = 1;

    framebuffer_info.width = window_ctx.display_ctx.base_extent.width;
    framebuffer_info.height = window_ctx.display_ctx.base_extent.height;
    framebuffer_info.layers = 1;

    for (uint32_t idx = 0; idx < image_count; idx++) {
        framebuffer_info.pAttachments = &window_ctx.display_ctx.swapchain_ctx.p_views[idx];
        res = vkCreateFramebuffer(*window_ctx.p_device, &framebuffer_info, NULL,
                                                            &p_framebuffers[idx]);

        if (res != VK_SUCCESS) {
            printf("framebuffer creation failure: %d\n", res);
            res = FAILURE;

            goto exit;
        }
    }

    window_ctx.display_ctx.framebuffer_ctx.p_framebufs = p_framebuffers;
    window_ctx.display_ctx.framebuffer_ctx.framebuffer_count = image_count;

    res = SUCCESS;

exit:
    return res;
}

uint32_t window_obj_mgr_get_framebuffer_object_count(void)
{
    return window_ctx.display_ctx.framebuffer_ctx.framebuffer_count;
}

VkFramebuffer *window_obj_mgr_get_framebuffer_object(uint32_t framebuffer_idx)
{
    if (framebuffer_idx >= window_obj_mgr_get_framebuffer_object_count()) {
        printf("invalid framebuffer index\n", framebuffer_idx);

        return FAILURE;
    }

    return &window_ctx.display_ctx.framebuffer_ctx.p_framebufs[framebuffer_idx];
}

void window_obj_mgr_exit(VkInstance *p_instance)
{
    window_ctx.ops.exit(&window_ctx.display_ctx.surface, p_instance);
    window_ctx.display_ctx.state = WINDOW_OBJ_DISPLAY_STATE_DESTROYED;
}
