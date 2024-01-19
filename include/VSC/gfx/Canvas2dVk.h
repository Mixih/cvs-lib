/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_CANVAS_2D_VK_H_
#define VSC_CANVAS_2D_VK_H_

#include <cstddef>
#include <memory>

#include <glad/vulkan.h>

namespace vsc {

/**
 * A 2D canvas in a window, drawn by Vulkan.
 */
class Canvas2DVk {

public:
    Canvas2DVk(std::size_t width, std::size_t height);
    ~Canvas2DVk();
};

} // namespace vsc

#endif /* VSC_CANVAS_2D_VK_H_ */
