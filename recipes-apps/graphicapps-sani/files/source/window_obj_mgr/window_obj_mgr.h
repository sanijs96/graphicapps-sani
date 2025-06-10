#ifndef __WINDOW_OBJ_MGR_H__
#define __WINDOW_OBJ_MGR_H__

#include <stdint.h>

#define DEFAULT_WINDOW_SIZE_WIDTH       (600)
#define DEFAULT_WINDOW_SIZE_HEIGHT      (300)

enum window_types {
    WINDOW_OBJ_TYPE_NONE = 0,
    WINDOW_OBJ_TYPE_GLFW,
    WINDOW_OBJ_TYPE_MAX,
};

uint32_t window_obj_mgr_init(uint32_t window_types);

void window_obj_mgr_resize(uint32_t width, uint32_t height);

void window_obj_mgr_start_display(uint32_t width, uint32_t height);

void window_obj_mgr_exit(void);

#endif
