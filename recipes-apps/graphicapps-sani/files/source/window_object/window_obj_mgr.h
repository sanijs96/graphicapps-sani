#ifndef __WINDOW_OBJ_MGR_H__
#define __WINDOW_OBJ_MGR_H__

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define DEFAULT_WINDOW_SIZE_WIDTH       (600)
#define DEFAULT_WINDOW_SIZE_HEIGHT      (300)

enum window_types {
    WINDOW_OBJ_TYPE_NONE = 0,
    WINDOW_OBJ_TYPE_GLFW,
    WINDOW_OBJ_TYPE_MAX,
};

enum window_surface_state {
    WINDOW_OBJ_DISPLAY_STATE_DEFAULT = 0,
    WINDOW_OBJ_DISPLAY_STATE_CREATED,
    WINDOW_OBJ_DISPLAY_STATE_STARTED,
    WINDOW_OBJ_DISPLAY_STATE_DESTROYED,
};

uint32_t window_obj_mgr_init(uint32_t window_types);

uint32_t window_obj_mgr_resize(uint32_t width, uint32_t height);

uint32_t window_obj_mgr_setup_display(VkInstance *p_instance);

uint32_t window_obj_mgr_start_display(VkDevice *p_device);

uint32_t window_obj_mgr_get_next_framebuffer_image_idx(void);

uint32_t window_obj_mgr_show_queue_result(VkQueue *p_queue);

uint32_t window_obj_mgr_check_display_status(void);

void window_obj_mgr_setup_device_ctx(VkPhysicalDevice *p_phydev, VkDevice *p_device);

uint32_t window_obj_mgr_show_display_ctx_info(VkPhysicalDevice *p_phydev);
VkSurfaceKHR *window_obj_mgr_get_current_display_object(void);

VkSwapchainKHR *window_obj_mgr_get_current_swapchain_object(void);
VkExtent2D *window_obj_mgr_get_current_swapchain_extent(void);
VkFormat *window_obj_mgr_get_current_swapchain_format(void);

uint32_t window_obj_mgr_create_framebuffers(VkRenderPass* p_renderpass);
uint32_t window_obj_mgr_get_framebuffer_object_count(void);
VkFramebuffer *window_obj_mgr_get_framebuffer_objects(void);

VkSemaphore *window_obj_mgr_get_display_semaphore_object(void);
void window_obj_mgr_add_swapchain_signal_semaphore(VkSemaphore *p_semaphore);

void window_obj_mgr_exit(VkInstance *p_instance);

#endif
