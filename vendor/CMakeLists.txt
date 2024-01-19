include(FetchContent)
set(FETCHCONTENT_QUIET off)
set (FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/vendor)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.4
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR cmake
)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
    GIT_SHALLOW TRUE
)

# settings
set(GLFW_BUILD_INSTALL OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

# now resolve all fetches
FetchContent_MakeAvailable(
    glm
    glad
    glfw
)

# upstream (vulkan) broke things witht he video headers, and now we have to use the
# outdata glad bundled headers to prevent breakages.
glad_add_library(glad_vk_12 SHARED LOADER REPRODUCIBLE API vulkan=1.2)
