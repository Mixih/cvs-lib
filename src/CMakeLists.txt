# SPDX-FileCopyrightText:  (C) 2024 Max Hahn
# SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
file(GLOB_RECURSE CVSLIB_SRCS *.cpp)

add_library(vsc ${CVSLIB_SRCS})
configure_target_with_defaults(vsc)
target_link_libraries(vsc
    PRIVATE glm
    PUBLIC glfw
    PUBLIC glad_vk_12)
