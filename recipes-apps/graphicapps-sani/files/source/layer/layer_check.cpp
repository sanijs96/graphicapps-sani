#include <iostream>
#include <cstring>
#include <vector>

#include "layer_check.hpp"

vulkanLayer::vulkanLayer(void)
: supportedLayerCount {0}, pSupportedLayers {nullptr}
{
    // get number of layers supported
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

    // get information of supported layers
    pSupportedLayers = new VkLayerProperties[supportedLayerCount];
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, pSupportedLayers);

    layerNamesList = new char *[supportedLayerCount];
    for (uint32_t layerIdx = 0; layerIdx < supportedLayerCount; layerIdx++) {
        layerNamesList[layerIdx] = pSupportedLayers->layerName;
    }
}

uint32_t vulkanLayer::getSupportedLayerCount(void)
{
    return supportedLayerCount;
}

char **vulkanLayer::getLayerNamesList(void)
{
    if (layerNamesList == nullptr) {
        throw std::runtime_error(" layer not detected ");
    }

    return layerNamesList;
}

bool vulkanLayer::checkLayerSupport(const char *layerName)
{
    for (uint32_t idx = 0; idx < supportedLayerCount; idx++) {
        if (!strcmp(layerName, pSupportedLayers[idx].layerName)) {
            return true;
        }
    }

    return false;
}
