cmake_minimum_required(VERSION 3.28)

# GLFW
set(BUILD_SHARED_LIBS  OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory("./glfw/")

# Glad
project(glad)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_BUILD_TYPE Release)

set(P_SOURCE_FILES
    "./glad/src/glad.c"
    )

set(P_HEADER_FILES
    "./glad/include/glad/glad.h"
    "./glad/include/KHR/khrplatform.h"
    )

add_library(${PROJECT_NAME} STATIC ${P_SOURCE_FILES} ${P_HEADER_FILES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC "./glad/include/"
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE -static-libstdc++
    )

# ImGui
project(imgui)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(P_SOURCE_FILES
    "./imgui/imgui.cpp"
    "./imgui/imgui_demo.cpp"
    "./imgui/imgui_draw.cpp"
    "./imgui/imgui_tables.cpp"
    "./imgui/imgui_widgets.cpp"
    "./imgui/backends/imgui_impl_glfw.cpp"
    "./imgui/backends/imgui_impl_opengl3.cpp"
    )

set(P_HEADER_FILES
    "./imgui/imgui.h"
    "./imgui/imconfig.h"
    "./imgui/imgui_internal.h"
    "./imgui/imstb_rectpack.h"
    "./imgui/imstb_textedit.h"
    "./imgui/imstb_truetype.h"
    "./imgui/backends/imgui_impl_glfw.h"
    "./imgui/backends/imgui_impl_opengl3.h"
    )

add_library(${PROJECT_NAME} STATIC ${P_SOURCE_FILES} ${P_HEADER_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC "./imgui/"
    PUBLIC "./glfw/include/"
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE -static-libstdc++
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

# Glm
add_subdirectory("./glm/")
