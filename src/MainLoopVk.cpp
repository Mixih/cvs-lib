/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#include "VSC/gfx/MainLoopVk.h"

#include <cstddef>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace vsc {

MainLoopVk::MainLoopVk()
    : instance([](VkInstance instance) { vkDestroyInstance(instance, nullptr); }) {
    init();
}

MainLoopVk::MainLoopVk(std::size_t initialSize)
    : instance([](VkInstance instance) { vkDestroyInstance(instance, nullptr); }) {
    windows.reserve(initialSize);
    init();
}

void MainLoopVk::init() {
    glfwInit();
    // load in global vk functions
    gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
    instance.takeOwnership(createInstance());
    // load in instance vk functions
    gladLoaderLoadVulkan(instance, nullptr, nullptr);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // uses opengl by default, so disable
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

VkInstance MainLoopVk::createInstance() {
    uint32_t glfwExtCnt = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCnt);

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Dummy",
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .pEngineName = "No engine",
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .apiVersion = VK_API_VERSION_1_2,
    };
    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = glfwExtCnt,
        .ppEnabledExtensionNames = glfwExtensions,
    };

    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
    return instance;
}

void MainLoopVk::runLoop() {
    bool shouldRun = windows.size() > 0;
    while (shouldRun) {
        for (WindowVk& window : windows) {
            window.render();
            shouldRun &= !window.shouldClose();
        }
        glfwPollEvents();
    }
}

} // namespace vsc
