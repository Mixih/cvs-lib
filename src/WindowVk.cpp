/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#include "VSC/gfx/WindowVk.h"

#include <cstddef>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace vsc {

WindowVk::WindowVk(std::size_t width, std::size_t height)
    : window(makeScopedResource<GLFWwindow*>(
          [](GLFWwindow* wind) { glfwDestroyWindow(wind); },
          [](std::size_t width, std::size_t height) {
              return glfwCreateWindow(width, height, "my first window", nullptr, nullptr);
          },
          width, height)) {
}

WindowVk::~WindowVk() {
}

void WindowVk::render() {
}

} // namespace vsc
