/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */

#ifndef DEVICE_SETUP_VK_H_
#define DEVICE_SETUP_VK_H_

#include <cstddef>
#include <vector>

#include <glad/vulkan.h>

#include "VSC/gfx/EnumeratorVk.h"
#include "VSC/gfx/WindowVk.h"
#include "VSC/util/ScopedResource.h"

namespace vsc {

class DeviceSetupVk {
private:
    // WARNING: To ensure that resources are released in the right order, mind the
    // ordering of the following class members.
    ScopedResource<VkInstance> instance;
    ScopedResource<VkDebugUtilsMessengerEXT, DeviceSetupVk*> debugMessenger;
    VkPhysicalDevice physicalDev; // implicitly destroyed with VkInstance
    ScopedResource<VkDevice> device;
    VkQueue graphicsQueue; // implicitly destroyed with device
    // END ORDER_CRITICAL
    bool enableValidationLayers;

    // functions
    void initGlfw();
    void initVk();
    void createInstance();
    std::vector<const char*> getRequiredExtensions() const;
    void createDebugMessenger();
    void createLogicalDevice(const PhysicalDeviceSelectionVk &devSel);

    // Utility static functions
    static VkDebugUtilsMessengerCreateInfoEXT resolveDbgMessengerCreateInfo();
    static bool checkValidationLayerSupport();

public:
    DeviceSetupVk(bool debug = false);
    void runLoop();
    VkInstance getInstance();
    VkPhysicalDevice getPhysicalDevice();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

} // namespace vsc

#endif /* DEVICE_SETUP_VK_H_ */
