#ifndef __LAYERS_CHECK_H__
#define __LAYERS_CHECK_H__

#include <vulkan/vulkan_core.h>

class vulkanLayer {
public:
    bool checkLayerSupport(const char* layerName);
    uint32_t getSupportedLayerCount(void);
    void addLayerNamesTo(const char* const *ppEnabledLayerNames);

    vulkanLayer(void);
    ~vulkanLayer(void);

private:
    uint32_t supportedLayerCount;
    VkLayerProperties *pSupportedLayers;
};

#endif
