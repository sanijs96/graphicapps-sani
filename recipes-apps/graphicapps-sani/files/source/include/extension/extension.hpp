#ifndef __EXTENSIONS_CHECK_H__
#define __EXTENSIONS_CHECK_H__

#include <stdint.h>

class vulkanExtension {
public:
    virtual uint32_t getRequiredExtensionCount(void) = 0;
    virtual const char ** getRequiredExtensionNames(void) = 0;
};

#endif
