cmake_minimum_required(VERSION 3.20)
project(ml_chess_engine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Only 64-bit builds are supported!")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compilation settings for Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
    add_compile_options( 
        -g                               # Generate debugging information
        -O0                              # No optimization
        -Wall                            # Enable basic warnings
        -Wextra                          # Enable additional warnings
        -Wpedantic                       # Enable strict standard warnings
    )
endif()

# Compilation settings for Release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(_RELEASE NDEBUG)
    add_compile_options(
        -O3                   # Aggressive optimization
        -march=native         # CPU-specific instructions
        -mtune=native         # Tune for current CPU
        -flto                 # Link-time optimization
        -flto-partition=one   # Single LTO partition
        -funroll-loops        # Unroll loops
        -fomit-frame-pointer  # Free up a register
    )
    add_link_options(-flto -flto-partition=one)
endif()

enable_testing()

add_subdirectory(test_utils)
add_subdirectory(modules/chess)
add_subdirectory(modules/nn)
add_subdirectory(src)
