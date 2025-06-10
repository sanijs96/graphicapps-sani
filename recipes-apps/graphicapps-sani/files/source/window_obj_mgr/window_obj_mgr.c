
#include "common/common_def.h"
#include "window_obj_mgr.h"

#if defined(GLFW_INCLUDE_VULKAN)
#include "glfw/window_glfw.h"
#endif

static struct {
    void (* create)(void);
    void (* display)(uint32_t width, uint32_t height);
    void (* resize)(uint32_t width, uint32_t height);
    void (* exit)(void);
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

void window_obj_mgr_resize(uint32_t width, uint32_t height)
{
    window_object.resize(width, height);
}

void window_obj_mgr_start_display(uint32_t width, uint32_t height)
{
    window_object.display(width, height);
}

void window_obj_mgr_exit(void)
{
    window_object.exit();
}

