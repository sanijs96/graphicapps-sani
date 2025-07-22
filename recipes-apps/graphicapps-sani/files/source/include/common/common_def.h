#ifndef __COMMON_DEFINITION_H__
#define __COMMON_DEFINITION_H__

#include <stdint.h>

#define SUCCESS                     (0)
#define FAILURE                     (UINT32_MAX)

#define FALSE                       (0)
#define TRUE                        (1)

#define DEBUG_EN                    (TRUE)
#define GLFW_INCLUDE_VULKAN         (TRUE)

#define BITS_PER_BYTE               (8)
#define BITS_PER_UINT32             (sizeof(uint32_t) * BITS_PER_BYTE)

#define MAX_LENGTH_ARGUMENT_NAME    (100)

#endif
