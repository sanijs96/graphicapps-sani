#ifndef __VULKAN_DEVICE_H__
#define __VULKAN_DEVICE_H__

#include <vulkan/vulkan_core.h>

class vulkanDevice {
public:
    vulkanDevice(void): deviceCount{0}, deviceNamesList{nullptr},
                        currentName{nullptr} { }

    ~vulkanDevice(void) { delete pPhysicalDeviceList; delete []deviceNamesList; }

    VkPhysicalDevice *createDeviceList(uint32_t count);
    void registerDevices(uint32_t count);

    uint32_t getDeviceCount(void);

    bool queueTypeSupported(char * deviceName, VkQueueFlagBits queueFlagBits);
    bool memoryTypeSupported(char * deviceName, VkMemoryPropertyFlagBits memoryFlagBits);

    const char *getDeviceName(uint32_t deviceIdx);
    bool isDeviceTypeGPU(uint32_t deviceIdx);
    void selectDevice(uint32_t deviceIdx);

private:
    uint32_t deviceCount;
    const char ** deviceNamesList;
    VkPhysicalDevice * pPhysicalDeviceList;

    const char * currentName;
    VkPhysicalDeviceFeatures feature;
    VkPhysicalDeviceProperties property;

    void createLogicalDevice(void);
};

#endif
