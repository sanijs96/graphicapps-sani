#ifndef __VULKAN_OBJ_MGR_H__
#define __VULKAN_OBJ_MGR_H__

#include <vulkan/vulkan_core.h>

typedef struct objNamesList {
    objNamesList *next;
    const char * name;
} objNamesList_t;

class vulkanObjectManager {
public:
    VkResult result;
    vulkanObjectManager(void): vkResult{VK_SUCCESS}, vkInstance{},
                                vkAppsInfo{}, vkInstanceInfo{} { }
    ~vulkanObjectManager(void);

    void createInstance(void);
    void destroyInstance(void);

    uint32_t getEnabledInstanceLayerCount(void);
    void addInstanceLayerInfo(const char *layerName);
    void deleteInstanceLayerInfo(const char *layerName);

    uint32_t getEnabledInstanceExtensionCount(void);
    void addInstanceExtensionInfo(const char *extNames);
    void deleteInstanceExtensionInfo(const char *extNames);

    void addObjNamesListEntry(objNamesList_t *pList, const char *name);
    void deleteObjNamesListEntry(objNamesList_t *pList);

private:
    objNamesList_t layerNamesList;
    objNamesList_t extensionNamesList;
    const char ** ppLayerNames;
    const char ** ppExtensionNames;
    VkResult vkResult;
    VkInstance vkInstance;
    VkApplicationInfo vkAppsInfo;
    VkInstanceCreateInfo vkInstanceInfo;

    void fill_vkApplicationInfo(void);

    void updateInstanceLayerNamesList(void);
    void updateInstanceExtensionNamesList(void);
};

#endif