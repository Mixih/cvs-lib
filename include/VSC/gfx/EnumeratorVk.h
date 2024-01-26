#ifndef VSC_ENUMERATOR_VK_H_
#define VSC_ENUMERATOR_VK_H_

#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glad/vulkan.h>

namespace vsc {

template <typename PropsT> class SingletonEnumeratorVk {
private:
    static SingletonEnumeratorVk* instance;

protected:
    std::vector<PropsT> cache;

    virtual uint32_t getResourceCount() = 0;
    virtual void getResources(uint32_t resourceCnt, std::vector<PropsT>& destVec) = 0;
    virtual SingletonEnumeratorVk* createSingleton() = 0;

    SingletonEnumeratorVk();

public:
    static SingletonEnumeratorVk& get();
    bool hasResource(std::string_view resourceName);
    bool updateCache(std::string_view resourceName);
};

/**
 * Caching enumerator singleton for vulkan layer lookups.
 *
 * NOT THREAD SAFE!
 */
class ExtEnumeratorVk {
private:
    static ExtEnumeratorVk* instance;
    std::vector<VkExtensionProperties> extensionCache;

    ExtEnumeratorVk() { updateCache(); }

public:
    /**
     * Get the singleton instance for use.
     */
    static ExtEnumeratorVk& get();
    /**
     * Check if the queried extension is present in the Vulkan runtime.
     */
    bool hasExt(std::string_view extName);
    /**
     * Update the cached list of extensions.
     */
    void updateCache();
};

/**
 * Enumerate layers that are supported by the Vulkan runtime.
 */
class LayerEnumeratorVk {
private:
    static LayerEnumeratorVk* instance;
    std::vector<VkLayerProperties> layerCache;

    LayerEnumeratorVk() { updateCache(); };

public:
    /**
     * Get the singleton instance for use.
     */
    static LayerEnumeratorVk& get();
    /**
     * Query for the presence of the requested layerName in the runtime.
     */
    bool hasLayer(std::string_view layerName);
    /**
     * Update cached list of layers.
     */
    void updateCache();
};

struct PhysicalDeviceDataVk {
    std::vector<VkQueueFamilyProperties> queueFamilies;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
};

struct PhysicalDeviceSelectionVk {
    int score = 0;
    VkPhysicalDevice dev = VK_NULL_HANDLE;
    std::optional<uint32_t> graphicsQueueFamilyIdx;
    std::optional<uint32_t> presentationQueueFamilyIdx;

    bool hasAllRequiredQueues();
};

class PhysicalDeviceEnumeratorVk {
private:
    VkInstance instance;
    VkSurfaceKHR surface;
    std::unordered_map<VkPhysicalDevice, PhysicalDeviceDataVk> deviceDataCache;

    void updateCache();
    PhysicalDeviceSelectionVk scorePhysDevice(VkPhysicalDevice dev,
                                              const PhysicalDeviceDataVk& devData);

public:
    PhysicalDeviceEnumeratorVk(VkInstance instance, VkSurfaceKHR surface);
    PhysicalDeviceSelectionVk selectPhysDevice();
};

} // namespace vsc

#endif /* VSC_ENUMERATOR_VK_H_ */
