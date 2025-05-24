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
    void addInstanceExtensionInfo(const char **extNames, uint32_t extCount);

    void addObjNamesListEntry(objNamesList_t *pList, const char *layerName);
    void deleteObjNamesListEntry(objNamesList_t *pList);
    void updateInstanceNamesPtr(const char * const *namePtr);

    void updateInstanceLayerNamesList(void);
private:
    objNamesList_t layerNamesList;
    VkResult vkResult;
    VkInstance vkInstance;
    VkApplicationInfo vkAppsInfo;
    VkInstanceCreateInfo vkInstanceInfo;


    void fill_vkApplicationInfo(void);
};

#endif