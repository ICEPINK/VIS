cmake_minimum_required(VERSION 3.28)

project(VIS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory("./lib/")

set(P_SOURCE_FILES
    "./src/main.cpp"
    "./src/application.cpp"
    "./src/window.cpp"
    "./src/gui.cpp"
    "./src/image.cpp"
    "./src/cpu_renderer.cpp"
    )

set(P_HEADER_FILES
    "./src/application.hpp"
    "./src/window.hpp"
    "./src/gui.hpp"
    "./src/image.hpp"
    "./src/cpu_renderer.hpp"
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
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
