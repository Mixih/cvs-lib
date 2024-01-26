/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#include "VSC/gfx/WindowVk.h"

#include <cstddef>
#include <stdexcept>
#include <string_view>
#include "VSC/util/Exception.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace vsc {

WindowVk::WindowVk(std::string name, std::size_t width, std::size_t height, VkInstance instance,
                   VkDevice device)
    : name(std::move(name)),
      instance{instance},
      device{device},
      window(makeScoped<GLFWwindow*>(
          [](GLFWwindow* wind) { glfwDestroyWindow(wind); },
          [this](std::size_t width, std::size_t height) {
              return glfwCreateWindow(width, height, this->name.c_str(), nullptr, nullptr);
          },
          width, height)),
      surface(
          [](VkSurfaceKHR surface, WindowVk* wind) {
              vkDestroySurfaceKHR(wind->instance, surface, nullptr);
          },
          this) {
    createSurface();
}

void WindowVk::createSurface() {
    VkSurfaceKHR surf;
    if (glfwCreateWindowSurface(instance, window.get(), nullptr, &surf) != VK_SUCCESS)
        throw VkRuntimeError("Failed to create surface for window.");
    surface.takeOwnership(std::move(surf));
}

WindowVk::~WindowVk() {
}

void WindowVk::render() {
}

} // namespace vsc
