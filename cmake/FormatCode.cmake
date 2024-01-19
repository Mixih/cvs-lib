# SPDX-FileCopyrightText:  (C) 2024 Max Hahn
# SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
include_guard(GLOBAL)

# function that registers a clang-format target for automatic code reformatting.
# user specifies the target name and the source directories to recursively reformat
function(register_format_code_target tgt_name source_dirs)
    set(src_files "")
    foreach(path ${source_dirs})
        file(GLOB_RECURSE srcs_in_dir
             "${path}/*.c"
             "${path}/*.cpp"
             "${path}/*.h"
             "${path}/*.inl")
        list(APPEND src_files ${srcs_in_dir})
    endforeach()
    add_custom_target(${tgt_name}
        COMMAND clang-format -i --verbose ${src_files}
        SOURCES .clang-format
    )
endfunction()
