#ifndef __LAYERS_CHECK_H__
#define __LAYERS_CHECK_H__

#include <stdint.h>
#include <vulkan/vulkan_core.h>

class vulkanLayer {
public:
    vulkanLayer(void);
    virtual bool checkLayerSupport(const char* layerName);
    virtual uint32_t getSupportedLayerCount(void);
    virtual char **getLayerNamesList(void);

private:
    char **layerNamesList;
    uint32_t supportedLayerCount;
    VkLayerProperties *pSupportedLayers;
};

#endif
