#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>
#include "common/common_def.h"

#if !defined(DEBUG_EN)
const uint32_t enable_validation = FALSE;
#else
const uint32_t enable_validation = TRUE;
const char *const validation_layer_names_list[] = {
    "VK_LAYER_KHRONOS_validation"
};
#endif

#endif
