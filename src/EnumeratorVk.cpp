#include "VSC/gfx/EnumeratorVk.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>

#include "VSC/util/Exception.h"

namespace vsc {

ExtEnumeratorVk* ExtEnumeratorVk::instance = nullptr;

ExtEnumeratorVk& ExtEnumeratorVk::get() {
    if (!instance) {
        instance = new ExtEnumeratorVk();
    }
    return *instance;
}

void ExtEnumeratorVk::updateCache() {
    uint32_t availableExtCnt;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtCnt, nullptr) !=
        VK_SUCCESS) {
        throw VkRuntimeError("Failed to enumerate Vulkan extension count.");
    }
    if (availableExtCnt == 0) {
        throw VkRuntimeError("No vulkan extensions are available.");
    }
    extensionCache.resize(availableExtCnt);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtCnt,
                                               extensionCache.data()) != VK_SUCCESS) {
        throw VkRuntimeError("Failed to retrieve Vulkan extension props.");
    }
}

bool ExtEnumeratorVk::hasExt(std::string_view extName) {
    auto it = std::find_if(extensionCache.begin(), extensionCache.end(),
                           [&extName](const VkExtensionProperties& prop) {
                               return extName == prop.extensionName;
                           });
    return it != extensionCache.end();
}

LayerEnumeratorVk* LayerEnumeratorVk::instance = nullptr;

LayerEnumeratorVk& LayerEnumeratorVk::get() {
    if (!instance) {
        instance = new LayerEnumeratorVk();
    }
    return *instance;
}

void LayerEnumeratorVk::updateCache() {
    uint32_t availableLayerCnt;
    if (vkEnumerateInstanceLayerProperties(&availableLayerCnt, nullptr) != VK_SUCCESS) {
        throw VkRuntimeError("Failed to enumerate Vulkan layer count.");
    }
    layerCache.resize(availableLayerCnt);
    if (vkEnumerateInstanceLayerProperties(&availableLayerCnt, layerCache.data()) !=
        VK_SUCCESS) {
        throw VkRuntimeError("Failed to retrieve Vulkan layer props.");
    }
}

bool LayerEnumeratorVk::hasLayer(std::string_view extName) {
    auto it = std::find_if(
        layerCache.begin(), layerCache.end(),
        [&extName](const VkLayerProperties& prop) { return extName == prop.layerName; });
    return it != layerCache.end();
}

bool PhysicalDeviceSelectionVk::hasAllRequiredQueues() {
    return graphicsQueueFamilyIdx.has_value() && presentationQueueFamilyIdx.has_value();
}

PhysicalDeviceEnumeratorVk::PhysicalDeviceEnumeratorVk(VkInstance instance)
    : instance(instance) {
    updateCache();
}

void PhysicalDeviceEnumeratorVk::updateCache() {
    uint32_t physDeviceCnt = 0;
    if (vkEnumeratePhysicalDevices(instance, &physDeviceCnt, nullptr) != VK_SUCCESS)
        throw VkRuntimeError("Failed to enumerate Vulkan devices");
    if (physDeviceCnt == 0)
        throw VkRuntimeError("No GPUs found with Vulkan support.");
    std::vector<VkPhysicalDevice> physDevices(physDeviceCnt);
    if (vkEnumeratePhysicalDevices(instance, &physDeviceCnt, physDevices.data()) !=
        VK_SUCCESS)
        throw VkRuntimeError("Failed to retrieve Vulkan device data.");

    for (auto& dev : physDevices) {
        VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties(dev, &devProps);
        VkPhysicalDeviceFeatures devFeatures;
        vkGetPhysicalDeviceFeatures(dev, &devFeatures);
        uint32_t qfCnt;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &qfCnt, nullptr);
        std::vector<VkQueueFamilyProperties> qfProps(qfCnt);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &qfCnt, qfProps.data());
        deviceDataCache.insert({dev,
                                {.queueFamilies = std::move(qfProps),
                                 .properties = devProps,
                                 .features = devFeatures}});
    }
}

PhysicalDeviceSelectionVk PhysicalDeviceEnumeratorVk::selectPhysDevice() {
    std::multimap<int, PhysicalDeviceSelectionVk> deviceScoreBoard;

    for (auto& deviceDataPair : deviceDataCache) {
        PhysicalDeviceSelectionVk data =
            scorePhysDevice(deviceDataPair.first, deviceDataPair.second);
        deviceScoreBoard.insert(std::make_pair(data.score, std::move(data)));
    }

    if (deviceScoreBoard.rend()->first < 0) {
        throw VkRuntimeError("No suitable GPU devices found.");
    }
    return deviceScoreBoard.rend()->second;
}

PhysicalDeviceSelectionVk
PhysicalDeviceEnumeratorVk::scorePhysDevice(VkPhysicalDevice dev,
                                            const PhysicalDeviceDataVk& devData) {
    PhysicalDeviceSelectionVk devSel;
    devSel.dev = dev;

    uint32_t queueNum = 0;
    for (auto& qf : devData.queueFamilies) {
        if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            devSel.graphicsQueueFamilyIdx = queueNum;
    }

    // device selection rubric
    switch (devData.properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            devSel.score += 1000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            devSel.score += 100;
            break;
        default:
            devSel.score += 0;
            break;
    }
    devSel.score += devData.properties.limits.maxImageDimension2D;
    if (!devSel.hasAllRequiredQueues())
        devSel.score = -1;

    return devSel;
}

} // namespace vsc
