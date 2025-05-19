#include "debug.hpp"

#if !defined(DEBUG_EN)
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayerNamesList = {
    "VK_LAYER_KHRONOS_validation"
};
