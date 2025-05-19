#include <iostream>
#include <cstring>
#include <vector>

#include "layer_obj_mgr.hpp"

vulkanLayer::vulkanLayer(void)
{
    // get number of layers supported
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

    // get information of supported layers
    pSupportedLayers = new VkLayerProperties[supportedLayerCount];
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, pSupportedLayers);
}

vulkanLayer::~vulkanLayer(void)
{
    delete []pSupportedLayers;
}

uint32_t vulkanLayer::getSupportedLayerCount(void)
{
    return supportedLayerCount;
}

bool vulkanLayer::checkLayerSupport(const char *layerName)
{
    VkLayerProperties *pLayer;

    pLayer = pSupportedLayers;

    for (uint32_t idx = 0; idx < supportedLayerCount; idx++) {
        if (!strcmp(layerName, pLayer->layerName)) {
            return true;
        }

        pLayer++;
    }

    return false;
}

void vulkanLayer::addLayerNamesTo(const char* const *ppEnabledLayerNames)
{
    VkLayerProperties *pLayer;

    pLayer = pSupportedLayers;
    for (uint32_t idx = 0; idx < supportedLayerCount; idx++) {
        ppEnabledLayerNames = (const char* const *)pLayer->layerName;
        ppEnabledLayerNames++;
    }
}
