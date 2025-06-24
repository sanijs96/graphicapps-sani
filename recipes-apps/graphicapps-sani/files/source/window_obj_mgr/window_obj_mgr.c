
#include "common/common_def.h"
#include "window_obj_mgr.h"

#if defined(GLFW_INCLUDE_VULKAN)
#include "glfw/window_glfw.h"
#endif

typedef struct window_ops {
    uint32_t (* create)(void);
    void (* resize)(uint32_t width, uint32_t height);
    uint32_t (* display)(VkSurfaceKHR *p_surface, VkInstance *p_instance);
    void (* exit)(VkSurfaceKHR *p_surface, VkInstance *p_instance);
} window_ops_t;

static struct {
    window_ops_t ops;
    uint32_t type;
    uint32_t state;
    VkSurfaceKHR surface;
} window_object;

uint32_t window_obj_mgr_init(uint32_t window_types)
{
    if (window_types == WINDOW_OBJ_TYPE_NONE) {
        return FAILURE;
    }

#if defined(GLFW_INCLUDE_VULKAN)
    glfw_init();
    window_object.type = WINDOW_OBJ_TYPE_GLFW;
    window_object.ops.create = glfw_create_window;
    window_object.ops.resize = glfw_change_window_size;
    window_object.ops.display = glfw_display_window;
    window_object.ops.exit = glfw_destroy_window;
#endif

    if (!window_object.ops.create) {
        return FAILURE;
    }

    window_object.ops.create();

    window_object.state = WINDOW_DISPLAY_SURFACE_STATE_DEFAULT;

    if (window_object.ops.resize) {
        window_object.ops.resize(DEFAULT_WINDOW_SIZE_WIDTH, DEFAULT_WINDOW_SIZE_HEIGHT);
    }

    return SUCCESS;
}

uint32_t window_obj_mgr_resize(uint32_t width, uint32_t height)
{
    window_object.ops.resize(width, height);

    return SUCCESS;
}

uint32_t window_obj_mgr_start_display(VkInstance *p_instance)
{
    if (window_object.ops.display(&window_object.surface, p_instance) == SUCCESS) {
        window_object.state = WINDOW_DISPLAY_SURFACE_STATE_CREATED;
        return SUCCESS;
    }
    else {
        return FAILURE;
    }
}

uint32_t window_obj_mgr_check_display_status(void)
{
    return window_object.state;
}

static void __window_obj_mgr_show_display_capabilities(VkSurfaceCapabilitiesKHR *p_cap)
{
    printf("\nCapabilities\n");
    printf("\tminImageCount       : %u\n", p_cap->minImageCount);
    printf("\tmaxImageCount       : %u\n", p_cap->maxImageCount);

    printf("\tcurrentExtent       : %u, %u\n",
            p_cap->currentExtent.width, p_cap->currentExtent.height);
    printf("\tminImageExtent      : %u\n",
            p_cap->minImageExtent.width, p_cap->minImageExtent.height);
    printf("\tmaxImageExtent      : %u\n",
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
    VkSurfaceKHR *p_surface;
    VkSurfaceCapabilitiesKHR capabilities;

    format_cnt = 0;
    present_cnt = 0;
    p_surface = &window_object.surface;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*p_phydev, *p_surface, &capabilities);
    __window_obj_mgr_show_display_capabilities(&capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(*p_phydev, *p_surface, &format_cnt, NULL);
    VkSurfaceFormatKHR formats[format_cnt];
    vkGetPhysicalDeviceSurfaceFormatsKHR(*p_phydev, *p_surface, &format_cnt, formats);

    printf("\nFormats\n");
    for (uint32_t idx = 0; idx < format_cnt; idx++) {
        if ((formats[idx].format == VK_FORMAT_B8G8R8A8_SRGB) &&
            (formats[idx].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
            printf("\t [%u] Format VK_FORMAT_B8G8R8A8_SRGB: supported\n", idx);
            printf("\t [%u] Colorspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: supported\n", idx);

            break;
        }
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(*p_phydev, *p_surface, &present_cnt, NULL);
    VkPresentModeKHR present_modes[present_cnt];
    vkGetPhysicalDeviceSurfacePresentModesKHR(*p_phydev, *p_surface, &present_cnt, present_modes);
    __window_obj_mgr_show_display_present_modes(present_modes, present_cnt);

    return SUCCESS;
}

VkSurfaceKHR *window_obj_mgr_get_display_object(void)
{
    return &window_object.surface;
}

void window_obj_mgr_exit(VkInstance *p_instance)
{
    window_object.ops.exit(&window_object.surface, p_instance);
    window_object.state = WINDOW_DISPLAY_SURFACE_STATE_DESTROYED;
}
