# SPDX-FileCopyrightText:  (C) 2024 Max Hahn
# SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
#
# Notice: this is the standard cmake file used to build this project
# Refer to the following resources for modern cmake scripts:
#   - https://cliutils.gitlab.io/modern-cmake/
#   - https://coderefinery.github.io/cmake-workshop/
cmake_minimum_required(VERSION 3.11...3.27 FATAL_ERROR)
# bootstrap project
project(verilator_common_project VERSION 0.0.1 LANGUAGES C CXX)

# add our custom modules to the module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# include required scripts
include(PreventInTreeBuilds)
include(Util)

# cmake global settings (USE WITH GREAT CARE!)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project settings
set(warn_flags -Wall -Wextra -pedantic)
set(PROJ_CXX_VERISON cxx_std_20)
set(PROJ_C_VERSION c_std_17)
set(PROJ_FLAGS "${warn_flags}")
# configure global paths for project
get_filename_component(proj_include_abs_path include ABSOLUTE BASE_DIR ${PROJECT_SOURCE_DIR})
set(PROJ_INCLUDE_DIR "${proj_include_abs_path}")

# add source and vendor directories
add_subdirectory(vendor)
add_subdirectory(src)
