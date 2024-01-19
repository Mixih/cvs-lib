/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */

#ifndef VSC_MAIN_LOOP_VK_H_
#define VSC_MAIN_LOOP_VK_H_

#include <cstddef>
#include <vector>

#include <glad/vulkan.h>

#include "VSC/gfx/WindowVk.h"
#include "VSC/util/ScopedResource.h"

namespace vsc {

class MainLoopVk {
private:
    std::vector<WindowVk> windows;
    ScopedResource<VkInstance> instance;
    void init();
    // Utility static functions
    static VkInstance createInstance();

public:
    MainLoopVk();
    MainLoopVk(std::size_t initialSize);
    void runLoop();
    void registerWindow(WindowVk&& window) { windows.push_back(std::move(window)); }
};

} // namespace vsc

#endif /* VSC_MAIN_LOOP_VK_H_ */
