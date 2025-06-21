
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
} window_object = {0, };

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

VkSurfaceKHR *window_obj_mgr_get_display_object(void)
{
    return &window_object.surface;
}

void window_obj_mgr_exit(VkInstance *p_instance)
{
    window_object.ops.exit(&window_object.surface, p_instance);
    window_object.state = WINDOW_DISPLAY_SURFACE_STATE_DESTROYED;
}
