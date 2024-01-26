# SPDX-FileCopyrightText:  (C) 2024 Max Hahn
# SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
#
# Notice: this is the standard cmake file used to build this project
# Refer to the following resources for modern cmake scripts:
#   - https://cliutils.gitlab.io/modern-cmake/
#   - https://coderefinery.github.io/cmake-workshop/
cmake_minimum_required(VERSION 3.23...3.27 FATAL_ERROR)
# bootstrap project
project(vsc_project VERSION 0.0.1 LANGUAGES C CXX)

# Library Options
# Set to OFF in consumer if it will manage its own vendor deps. The includes glm, glfw,
# and glad
option(VSC_USE_VENDORED_DEPS "Download glx, glm, and glad" ON)
option(VSC_BUILD_EXAMPLES "Build example code" ON)

# add our custom modules to the module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# include required scripts
include(FormatCode)
include(PreventInTreeBuilds)
include(Util)

# CMake global settings (USE WITH GREAT CARE!)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# internal project settings (so we don't modify any CMake global settings.)
set(warn_flags -Wall -Wextra -pedantic)
set(PROJ_CXX_VERSION cxx_std_20)
set(PROJ_C_VERSION c_std_17)
set(PROJ_FLAGS "${warn_flags}")
# configure global paths for project
get_filename_component(proj_include_abs_path include ABSOLUTE BASE_DIR ${PROJECT_SOURCE_DIR})
set(PROJ_INCLUDE_DIR "${proj_include_abs_path}")

# Only resolve vendored libs if configured to
if (VSC_USE_VENDORED_DEPS)
    add_subdirectory(vendor)
endif()
add_subdirectory(src)
if (VSC_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# add format target
register_format_code_target("format" "example;include;src")
