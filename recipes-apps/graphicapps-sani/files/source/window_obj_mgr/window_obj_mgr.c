
#include "common/common_def.h"
#include "window_obj_mgr.h"

#if defined(GLFW_INCLUDE_VULKAN)
#include "glfw/window_glfw.h"
#endif

static struct {
    uint32_t (* create)(void);
    uint32_t (* display)(uint32_t width, uint32_t height, VkInstance *p_instance);
    uint32_t (* resize)(uint32_t width, uint32_t height, VkInstance *p_instance);
    void (* exit)(VkInstance *p_instance);
} window_object = {0, };

uint32_t window_obj_mgr_init(uint32_t window_types)
{
    if (window_types == WINDOW_OBJ_TYPE_NONE) {
        return FAILURE;
    }

#if defined(GLFW_INCLUDE_VULKAN)
    glfw_init();
    window_object.create = glfw_create_window;
    window_object.resize = glfw_resize_window;
    window_object.display = glfw_display_window;
    window_object.exit = glfw_destroy_window;
#endif

    if (!window_object.create) {
        return FAILURE;
    }

    window_object.create();

    return SUCCESS;
}

uint32_t window_obj_mgr_resize(uint32_t width, uint32_t height, VkInstance *p_instance)
{
    return window_object.resize(width, height, p_instance);
}

uint32_t window_obj_mgr_start_display(uint32_t width, uint32_t height, VkInstance *p_instance)
{
    if (width == 0) {
        width = DEFAULT_WINDOW_SIZE_WIDTH;
    }

    if (height == 0) {
        height = DEFAULT_WINDOW_SIZE_HEIGHT;
    }

    return window_object.display(width, height, p_instance);
}

void window_obj_mgr_exit(VkInstance *p_instance)
{
    window_object.exit(p_instance);
}
