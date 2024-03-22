cmake_minimum_required(VERSION 3.28)

project(VIS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

message("### CMAKE OPTIONS ###")
message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "CXX_STANDARD_REQUIRED: ${CMAKE_CXX_STANDARD_REQUIRED}")
message(STATUS "C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "C_STANDARD: ${CMAKE_C_STANDARD}")
message(STATUS "C_STANDARD_REQUIRED: ${CMAKE_C_STANDARD_REQUIRED}")
message(STATUS "EXPORT_COMPILE_COMMANDS: ${CMAKE_EXPORT_COMPILE_COMMANDS}")
message("")

add_subdirectory("./lib/")

set(P_SOURCE_FILES
    "./src/application.cpp"
    "./src/cpu_renderer.cpp"
    "./src/depth_buffer.cpp"
    "./src/gpu_api.cpp"
    "./src/gui.cpp"
    "./src/image.cpp"
    "./src/main.cpp"
    "./src/perspective_camera.cpp"
    "./src/pipeline.cpp"
    "./src/solids/axis.cpp"
    "./src/solids/cube.cpp"
    "./src/solids/grid.cpp"
    "./src/solids/solid.cpp"
    "./src/solids/square.cpp"
    "./src/texture.cpp"
    "./src/window.cpp"
    )

set(P_HEADER_FILES
    "./src/application.hpp"
    "./src/camera.hpp"
    "./src/cpu_renderer.hpp"
    "./src/depth_buffer.hpp"
    "./src/gpu_api.hpp"
    "./src/gui.hpp"
    "./src/image.hpp"
    "./src/pipeline.hpp"
    "./src/solids/axis.hpp"
    "./src/solids/cube.hpp"
    "./src/solids/grid.hpp"
    "./src/solids/solid.hpp"
    "./src/solids/square.hpp"
    "./src/texture.hpp"
    "./src/utils/app_info.hpp"
    "./src/utils/color.hpp"
    "./src/utils/scene_info.hpp"
    "./src/utils/vertex.hpp"
    "./src/window.hpp"
    )

add_executable(${PROJECT_NAME} ${P_SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "./lib/glfw/include/"
    PUBLIC "./lib/glad/include/"
    PUBLIC "./lib/glm/"
    PUBLIC "./lib/imgui/"
    PUBLIC "./lib/imgui/backends/"
    )

target_link_directories(${PROJECT_NAME}
    PRIVATE "./lib/glfw/src/"
    PRIVATE "./lib/glad/src/"
    PRIVATE "./lib/imgui/"
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE -static-libstdc++
    PRIVATE glfw
    PRIVATE glad
    PRIVATE glm
    PRIVATE imgui
    )

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CXX_COMPILER_ID:GNU>:-O3 -Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:Clang>:-O2 -Wall -Wextra -Wpedantic -Werror>
    # $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O2 -Wall -Wextra -Wpedantic -Werror>
)
