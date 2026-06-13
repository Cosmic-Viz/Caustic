cmake_minimum_required(VERSION 3.10)
project(Caustic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(caustic
    src/main.cpp
)

target_include_directories(caustic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compiler flags for optimization
if(MSVC)
    target_compile_options(caustic PRIVATE /O2 /W4)
else()
    target_compile_options(caustic PRIVATE -O3 -Wall -Wextra)
endif()
