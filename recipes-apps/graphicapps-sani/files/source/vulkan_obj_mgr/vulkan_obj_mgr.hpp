#ifndef __VULKAN_OBJ_MGR_H__
#define __VULKAN_OBJ_MGR_H__

#include <vulkan/vulkan_core.h>

#include "device/vulkan_device.hpp"

typedef struct objNamesList {
    objNamesList *next;
    const char * name;
} objNamesList_t;

class vulkanObjectManager {
public:
    VkResult result;
    vulkanObjectManager(void): result{VK_SUCCESS}, vkInstance{},
                                vkAppsInfo{}, vkInstanceInfo{} { }
    ~vulkanObjectManager(void);

    // instance
    void createInstance(void);
    void destroyInstance(void);

    // layers & extensions
    uint32_t getEnabledInstanceLayerCount(void);
    void addInstanceLayerInfo(const char *layerName);
    void deleteInstanceLayerInfo(const char *layerName);

    uint32_t getEnabledInstanceExtensionCount(void);
    void addInstanceExtensionInfo(const char *extNames);
    void deleteInstanceExtensionInfo(const char *extNames);

    // device select & creation
    uint32_t selectGPUDevice(void);

    // common
    void addObjNamesListEntry(objNamesList_t *pList, const char *name);
    void deleteObjNamesListEntry(objNamesList_t *pList);

private:
    VkInstance vkInstance;
    VkApplicationInfo vkAppsInfo;
    VkInstanceCreateInfo vkInstanceInfo;

    const char ** ppLayerNames;
    const char ** ppExtensionNames;
    objNamesList_t layerNamesList;
    objNamesList_t extensionNamesList;

    vulkanDevice vkDevice;

    void fillApplicationInfo(void);
    void checkAvailableDevices(void);

    void updateInstanceLayerNamesList(void);
    void updateInstanceExtensionNamesList(void);

};

#endif