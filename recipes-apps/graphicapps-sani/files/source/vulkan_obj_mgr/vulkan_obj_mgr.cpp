#include <cstring>
#include <iostream>

#include "vulkan_obj_mgr.hpp"

void vulkanObjectManager::fillApplicationInfo(void)
{
    vkAppsInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    vkAppsInfo.pApplicationName = "Sani Vulkan Application";
    vkAppsInfo.pEngineName = "No Engine";

    vkAppsInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vkAppsInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppsInfo.apiVersion = VK_API_VERSION_1_0;

    vkAppsInfo.pNext = nullptr; // used on pointing to next vulkan extension object
}

void vulkanObjectManager::checkAvailableDevices(void)
{
    uint32_t deviceCount;
    VkPhysicalDevice * deviceList;

    deviceCount = 0;

    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("no physical devices available..");
    }

    deviceList = vkDevice.createDeviceList(deviceCount);

    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, deviceList);

    vkDevice.registerDevices(deviceCount);
}

void vulkanObjectManager::createInstance(void)
{
    fillApplicationInfo();

    vkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceInfo.pApplicationInfo = &vkAppsInfo;

    result = vkCreateInstance(&vkInstanceInfo, nullptr, &vkInstance);

    vkInstanceInfo.enabledLayerCount = 0;
    vkInstanceInfo.enabledExtensionCount = 0;

    layerNamesList = (objNamesList_t) {
        .next {nullptr},
        .name {nullptr}
    };

    extensionNamesList = (objNamesList_t) {
        .next {nullptr},
        .name {nullptr}
    };

    ppLayerNames = {nullptr};
    ppExtensionNames  = {nullptr};

}

void vulkanObjectManager::destroyInstance(void)
{
    vkDestroyInstance(vkInstance, nullptr);
}

vulkanObjectManager::~vulkanObjectManager(void)
{
    objNamesList_t *pList;

    pList = &layerNamesList;
    while (pList->next != nullptr) {
        delete pList;
        pList = pList->next;
    }

    pList = &extensionNamesList;
    while (pList->next != nullptr) {
        delete pList;
        pList = pList->next;
    }

    delete []ppLayerNames;
    delete []ppExtensionNames;
}

void vulkanObjectManager::addObjNamesListEntry(objNamesList_t *pList, const char *entryName)
{

    if (pList->name != nullptr) {
        objNamesList_t *entry = new objNamesList_t;
        *entry = {
            .next {nullptr},
            .name {entryName}
        };

        pList->next = entry;
    }
    else {
        pList->name = entryName;
    }
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

    if (vkInstanceInfo.ppEnabledLayerNames != nullptr) {
        delete []ppLayerNames;
    }

    ppLayerNames = new const char * [vkInstanceInfo.enabledLayerCount];

    pList = &layerNamesList;
    for (uint32_t idx = 0; ; idx++) {
        ppLayerNames[idx] = pList->name;

        std::cout << "[" << ppLayerNames[idx] << "]" << std::endl;

        if (!pList->next) {
            break;
        }

        pList = pList->next;
    }
}

void vulkanObjectManager::updateInstanceExtensionNamesList(void)
{
    objNamesList_t *pList;

    if (vkInstanceInfo.ppEnabledExtensionNames != nullptr) {
        delete []ppExtensionNames;
    }

    ppExtensionNames = new const char * [vkInstanceInfo.enabledExtensionCount];

    pList = &extensionNamesList;
    for (uint32_t idx = 0; ; idx++) {
        ppExtensionNames[idx] = pList->name;

        std::cout << "[" << ppExtensionNames[idx] << "]" << std::endl;

        if (!pList->next) {
            break;
        }

        pList = pList->next;
    }

    vkInstanceInfo.ppEnabledExtensionNames = ppExtensionNames;
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
    objNamesList_t *pList;
    vkInstanceInfo.enabledLayerCount++;

    uint32_t idx;

    pList = &layerNamesList;
    while (pList->next != nullptr) {
        pList = pList->next;
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

    while (pList->next != nullptr) {
        if (!strcmp(layerName, pList->next->name)) {
            deleteObjNamesListEntry(pList->next);

            vkInstanceInfo.enabledLayerCount--;

            updateInstanceLayerNamesList();
        }

        pList = pList->next;
    }
}

void vulkanObjectManager::addInstanceExtensionInfo(const char * extensionName)
{
    vkInstanceInfo.enabledExtensionCount++;

    objNamesList_t *pList;

    pList = &extensionNamesList;

    while (pList->next != nullptr) {
        pList = pList->next;
    }

    addObjNamesListEntry(pList, extensionName);

    updateInstanceExtensionNamesList();
}



uint32_t vulkanObjectManager::selectGPUDevice(void)
{
    uint32_t numDevices;
    const char * deviceName;

    checkAvailableDevices();

    numDevices = vkDevice.getDeviceCount();

    for (uint32_t idx; idx < numDevices; idx++) {

        if (vkDevice.isDeviceTypeGPU(idx) == true) {
            vkDevice.selectDevice(idx);
            std::cout << vkDevice.getDeviceName(idx) << "is selected" << std::endl;

            result = VK_SUCCESS;
            return result;
        }
    }

    return VK_ERROR_UNKNOWN;
}
