/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#include "VSC/gfx/MainLoopVk.h"

#include <string.h>

#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

#include "VSC/gfx/EnumeratorVk.h"
#include "VSC/gfx/glfw_fwd.h"
#include "VSC/util/Exception.h"
#include "VSC/util/ScopedResource.h"
#include "VSC/util/Suppressions.h"

namespace {
const char* vkSevToStr(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    static const std::unordered_map<VkDebugUtilsMessageSeverityFlagBitsEXT, const char*>
        sevToStrMap{
            {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "DEBUG"},
            {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "INFO"},
            {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "WARN"},
            {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "ERROR"},
        };
    return sevToStrMap.at(severity);
}

const char* vkMsgTypeToStr(VkDebugUtilsMessageTypeFlagsEXT type) {
    static const std::unordered_map<VkDebugUtilsMessageTypeFlagsEXT, const char*>
        msgTypeToStrMap{
            {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "general"},
            {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "validation"},
            {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, "perf"},
        };
    return msgTypeToStrMap.at(type);
}

const std::array validationLayers{"VK_LAYER_KHRONOS_validation"};

} // namespace

namespace vsc {

DeviceSetupVk::DeviceSetupVk(bool debug)
    : instance(
          VK_NULL_HANDLE, [](auto instance) { vkDestroyInstance(instance, nullptr); },
          false),
      debugMessenger(
          VK_NULL_HANDLE,
          [](VkDebugUtilsMessengerEXT debugMessenger, DeviceSetupVk* loop) {
              if (loop->enableValidationLayers)
                  vkDestroyDebugUtilsMessengerEXT(loop->instance, debugMessenger,
                                                  nullptr);
          },
          this, false),
      physicalDev(VK_NULL_HANDLE),
      device(
          VK_NULL_HANDLE, [](VkDevice dev) { vkDestroyDevice(dev, nullptr); }, false),
      enableValidationLayers(debug) {
    initGlfw();
    initVk();
}

void DeviceSetupVk::initGlfw() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // uses opengl by default, so disable
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void DeviceSetupVk::initVk() {
    // stage 0.1: create and load the instance
    gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
    createInstance();
    gladLoaderLoadVulkan(instance, nullptr, nullptr);
    if (enableValidationLayers) {
        createDebugMessenger();
    }
    // stage 0.2: create and load the physical device
    // Need a dummy invisible window so we can enumerate which devices support outputting
    // to surfaces instead of pure compute queues.
    ScopedResource enumWind = makeScoped<GLFWwindow*>(
        [](GLFWwindow* wind) { glfwDestroyWindow(wind); },
        []() {
            glfwWindowHint(GLFW_VISIBLE, false);
            GLFWwindow *wind = glfwCreateWindow(100, 100, "dummy window", nullptr, nullptr);
            glfwWindowHint(GLFW_VISIBLE, true);
            return wind;
        });
    ScopedResource enumSurf = makeScoped<VkSurfaceKHR>(
        this,
        [](VkSurfaceKHR surf, DeviceSetupVk* ctx) {
            vkDestroySurfaceKHR(ctx->instance, surf, nullptr);
        },
        [this, &enumWind]() {
            VkSurfaceKHR surf;
            glfwCreateWindowSurface(instance, enumWind, nullptr, &surf);
            return surf;
        });
    PhysicalDeviceEnumeratorVk physDevEnumerator(instance, enumSurf);
    PhysicalDeviceSelectionVk selectedPhysDev = physDevEnumerator.selectPhysDevice();
    physicalDev = selectedPhysDev.dev;
    gladLoaderLoadVulkan(instance, physicalDev, nullptr);
    // stage 0.3: create and load the logical device
    createLogicalDevice(selectedPhysDev);
    gladLoaderLoadVulkan(instance, physicalDev, device);
}

void DeviceSetupVk::createInstance() {
    uint32_t enabledLayerCount = 0;
    const char* const* enabledLayerNames = nullptr;
    if (enableValidationLayers) {
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error(
                "Debug enabled but missing required validation layers.");
        }
        enabledLayerCount = validationLayers.size();
        enabledLayerNames = validationLayers.data();
    }

    std::vector<const char*> extensionNames = getRequiredExtensions();

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Main Vulkan Loop",
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .pEngineName = "No engine",
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .apiVersion = VK_API_VERSION_1_2,
    };

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo =
        resolveDbgMessengerCreateInfo();
    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = enableValidationLayers ? &debugMessengerCreateInfo : nullptr,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enabledLayerCount,
        .ppEnabledLayerNames = enabledLayerNames,
        .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
        .ppEnabledExtensionNames = extensionNames.data(),
    };

    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
    this->instance.takeOwnership(std::move(instance));
}

std::vector<const char*> DeviceSetupVk::getRequiredExtensions() const {
    uint32_t glfwExtCnt = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCnt);

    std::vector<const char*> extensionNames(glfwExtensions, glfwExtensions + glfwExtCnt);

    if (enableValidationLayers) {
        extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensionNames;
}

void DeviceSetupVk::createDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = resolveDbgMessengerCreateInfo();
    VkDebugUtilsMessengerEXT debugMessenger;
    if (vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) !=
        VK_SUCCESS)
        throw std::runtime_error("Failed to create vkDebugUtilsMessenger instance.");
    this->debugMessenger.takeOwnership(std::move(debugMessenger));
}

void DeviceSetupVk::createLogicalDevice(const PhysicalDeviceSelectionVk& devSel) {
    // deal with deprecated implementations by setting same device layers as instance
    // layers
    uint32_t enabledLayerCount = 0;
    const char* const* enabledLayerNames = nullptr;
    if (enableValidationLayers) {
        enabledLayerCount = validationLayers.size();
        enabledLayerNames = validationLayers.data();
    }

    float queuePriority = 1.0;
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = devSel.graphicsQueueFamilyIdx.value(),
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };
    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledLayerCount = enabledLayerCount,
        .ppEnabledLayerNames = enabledLayerNames,
        .pEnabledFeatures = &deviceFeatures,
    };

    VkDevice device;
    if (vkCreateDevice(physicalDev, &createInfo, nullptr, &device) != VK_SUCCESS)
        throw VkRuntimeError("Failed to create logical Vulkan device.");
    this->device.takeOwnership(std::move(device));
    vkGetDeviceQueue(this->device, devSel.graphicsQueueFamilyIdx.value(), 0,
                     &graphicsQueue);
}

VkDebugUtilsMessengerCreateInfoEXT DeviceSetupVk::resolveDbgMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DeviceSetupVk::debugCallback,
        .pUserData = nullptr,
    };
    return createInfo;
}

bool DeviceSetupVk::checkValidationLayerSupport() {
    for (const char* requiredLayerName : validationLayers) {
        if (!LayerEnumeratorVk::get().hasLayer(requiredLayerName))
            return false;
    }
    if (!ExtEnumeratorVk::get().hasExt(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        return false;
    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DeviceSetupVk::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    unused(pUserData);

    std::cerr << std::format("[VK_DEBUG_UTILS_MSG][{0}][{1}]: {2}\n",
                             vkSevToStr(messageSeverity), vkMsgTypeToStr(messageType),
                             pCallbackData->pMessage);
    return VK_FALSE;
}

// void SystemConnectionVk::runLoop() {
//     bool shouldRun = windows.size() > 0;
//     while (shouldRun) {
//         for (WindowVk& window : windows) {
//             window.render();
//             shouldRun &= !window.shouldClose();
//         }
//         glfwPollEvents();
//     }
// }

} // namespace vsc
