cmake_minimum_required(VERSION 3.28)

project(VIS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(P_SOURCE_FILES
    "./src/main.cpp"
    "./src/application.cpp"
    )

set(P_HEADER_FILES
    "./src/application.hpp"
    )

add_executable(${PROJECT_NAME} ${P_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE -static-libstdc++
    )

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
