# SPDX-FileCopyrightText:  (C) 2024 Max Hahn
# SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0

function(configure_target_with_defaults target)
    target_include_directories("${target}" PUBLIC ${PROJ_INCLUDE_DIR})
    target_compile_features("${target}" PUBLIC ${PROJ_CXX_VERSION})
    target_compile_options("${target}" PRIVATE ${PROJ_FLAGS})
    set_target_properties("${target}" PROPERTIES
        C_EXTENSIONS FALSE
        CXX_EXTENSIONS FALSE
    )
endfunction()
