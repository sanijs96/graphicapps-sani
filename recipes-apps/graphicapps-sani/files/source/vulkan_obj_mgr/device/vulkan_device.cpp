#include "vulkan_device.hpp"
#include <iostream>

VkPhysicalDevice *vulkanDevice::createDeviceList(uint32_t count)
{
    pPhysicalDeviceList = new VkPhysicalDevice [deviceCount];

    return pPhysicalDeviceList;
}

void vulkanDevice::registerDevices(uint32_t count)
{
    VkPhysicalDeviceProperties ppty;

    deviceCount = count;

    deviceNamesList = new const char * [count];
    for (uint32_t idx = 0; idx < count; idx++) {
        std::cout << "dev [" << idx << "] name:" << deviceNamesList[idx] << std::endl;

        vkGetPhysicalDeviceProperties(pPhysicalDeviceList[idx], &ppty);

        deviceNamesList[idx] = ppty.deviceName;
    }
}

uint32_t vulkanDevice::getDeviceCount(void)
{
    return deviceCount;
}

const char *vulkanDevice::getDeviceName(uint32_t deviceIdx)
{
    return deviceNamesList[deviceIdx];
}

bool vulkanDevice::isDeviceTypeGPU(uint32_t deviceIdx)
{
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceProperties deviceProperty;

    vkGetPhysicalDeviceFeatures(pPhysicalDeviceList[deviceIdx], &deviceFeatures);

    vkGetPhysicalDeviceProperties(pPhysicalDeviceList[deviceIdx], &deviceProperty);

    return (deviceProperty.deviceType == 
            (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader));
}

void vulkanDevice::createLogicalDevice(void)
{

}

void vulkanDevice::selectDevice(uint32_t deviceIdx)
{
    currentName = deviceNamesList[deviceIdx];
    vkGetPhysicalDeviceFeatures(pPhysicalDeviceList[deviceIdx], &feature);
    vkGetPhysicalDeviceProperties(pPhysicalDeviceList[deviceIdx], &property);
}

