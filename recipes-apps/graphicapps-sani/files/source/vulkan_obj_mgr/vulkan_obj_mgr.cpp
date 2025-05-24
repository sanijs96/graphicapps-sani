#include <cstring>

#include "vulkan_obj_mgr.hpp"

void vulkanObjectManager::fill_vkApplicationInfo(void)
{
    vkAppsInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    vkAppsInfo.pApplicationName = "Sani Vulkan Application";
    vkAppsInfo.pEngineName = "No Engine";

    vkAppsInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vkAppsInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppsInfo.apiVersion = VK_API_VERSION_1_0;

    vkAppsInfo.pNext = nullptr; // used on pointing to next vulkan extension object
}

void vulkanObjectManager::createInstance(void)
{
    fill_vkApplicationInfo();

    vkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceInfo.pApplicationInfo = &vkAppsInfo;

    result = vkCreateInstance(&vkInstanceInfo, nullptr, &vkInstance);

    vkInstanceInfo.enabledLayerCount = 0;
    vkInstanceInfo.enabledExtensionCount = 0;

    layerNamesList = (objNamesList_t) {
        .next {nullptr},
        .name {nullptr}
    };

}

void vulkanObjectManager::destroyInstance(void)
{
    vkDestroyInstance(vkInstance, nullptr);
}

vulkanObjectManager::~vulkanObjectManager(void)
{
    objNamesList_t *pList;

    pList = &layerNamesList;
    for (uint32_t idx = 0; ; idx++) {
        if (!pList->next) {
            break;
        }

        delete pList;

        pList = pList->next;
    }
}

void vulkanObjectManager::addObjNamesListEntry(objNamesList_t *pList, const char *layerName)
{
    objNamesList_t *entry = new objNamesList_t;

    *entry = {
        .next {nullptr},
        .name {layerName}
    };

    pList->next = entry;
}

void vulkanObjectManager::deleteObjNamesListEntry(objNamesList_t *pList)
{
    if (!pList->next) {
        pList->next = pList->next->next;
    }
    else {
        pList->next = nullptr;
    }
}


void vulkanObjectManager::updateInstanceLayerNamesList(void)
{
    objNamesList_t *pList;
    const char * ppLayerNames[vkInstanceInfo.enabledLayerCount];

    pList = &layerNamesList;
    for (uint32_t idx = 0; ; idx++) {
        ppLayerNames[idx] = pList->name;

        if (!pList->next) {
            break;
        }

        pList = (objNamesList_t *)pList->next;
    }
}

uint32_t vulkanObjectManager::getEnabledInstanceLayerCount(void)
{
    return vkInstanceInfo.enabledLayerCount;
}

uint32_t vulkanObjectManager::getEnabledInstanceExtensionCount(void)
{
    return vkInstanceInfo.enabledExtensionCount;
}

void vulkanObjectManager::addInstanceLayerInfo(const char * layerName)
{
    vkInstanceInfo.enabledLayerCount++;

    uint32_t idx;
    objNamesList_t *pList;

    pList = &layerNamesList;
    for (; idx < vkInstanceInfo.enabledLayerCount - 1; idx++) {
        pList = (objNamesList_t *)pList->next;
    }

    addObjNamesListEntry(pList, layerName);

    updateInstanceLayerNamesList();
}

void vulkanObjectManager::deleteInstanceLayerInfo(const char *layerName)
{
    uint32_t idx;
    objNamesList_t *pList;

    idx = 0;
    *pList = layerNamesList;

    if (!strcmp(layerName, pList->name)) {
        deleteObjNamesListEntry(pList);

        vkInstanceInfo.enabledLayerCount--;

        updateInstanceLayerNamesList();
        return;
    }

    for (; idx < vkInstanceInfo.enabledLayerCount; idx++) {
        if (!strcmp(layerName, pList->next->name)) {
            break;
        }

        pList = pList->next;
    }

    if ((pList->next != nullptr)) {
        deleteObjNamesListEntry(pList->next);

        vkInstanceInfo.enabledLayerCount--;

        updateInstanceLayerNamesList();
    }
}

void vulkanObjectManager::addInstanceExtensionInfo(const char **extNames, uint32_t extCount)
{
    vkInstanceInfo.enabledExtensionCount += extCount;
    vkInstanceInfo.ppEnabledExtensionNames = extNames;
}

