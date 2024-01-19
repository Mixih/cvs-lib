/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_WINDOW_VK_H_
#define VSC_WINDOW_VK_H_

#include <cstddef>

#include "VSC/util/ScopedResource.h"

#include "glfw_fwd.h"

namespace vsc {

class WindowVk {
private:
    std::size_t layers;
    // note: order matters here! The cleanup will run in reverse order these resources are
    // declared here, so DO NOT reorder these definitions.
    // BEGIN STRICT_ORDERING
    ScopedResource<GLFWwindow*> window;
    // END STRICT_ORDERING
public:
    WindowVk(WindowVk&&) = default;
    WindowVk& operator=(WindowVk&&) = default;

    WindowVk(std::size_t width, std::size_t height);
    ~WindowVk();

    void initWindow();
    void initVulkan();
    bool shouldClose() const {
        // required for compatibility with C's looser const semantics.
        return glfwWindowShouldClose(const_cast<GLFWwindow*>(window.get()));
    }
    /**
     * Render call
     */
    void render();
};

} // namespace vsc

#endif /* VSC_WINDOW_VK_H_ */
