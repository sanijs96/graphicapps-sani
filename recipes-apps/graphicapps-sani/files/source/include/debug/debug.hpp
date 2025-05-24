#ifndef __DEBUG_H__
#define __DEBUG_H__

#if !defined(DEBUG_EN)
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
const char *const validationLayerNamesList[] = {
    "VK_LAYER_KHRONOS_validation"
};
#endif

#endif
