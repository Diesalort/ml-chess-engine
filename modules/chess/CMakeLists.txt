cmake_minimum_required(VERSION 3.10)
project(chess_logic CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mbmi2" COMPILER_SUPPORTS_BMI2)

add_library(chess
    src/board.cpp
    src/coordinates.cpp
    src/history.cpp
    src/move_generator_magic_bitboards.cpp
    src/perft.cpp
    src/precomputed_move_data.cpp
)

target_include_directories(chess PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compilation settings for Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chess PRIVATE _DEBUG)
    target_compile_options(chess PRIVATE 
        -g                               # Generate debugging information
        -O0                              # No optimization
        -Wall                            # Enable basic warnings
        -Wextra                          # Enable additional warnings
        -Wpedantic                       # Enable strict standard warnings
    )
endif()

# Compilation settings for Release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(chess PRIVATE _RELEASE NDEBUG)
    set(RELEASE_FLAGS
        -Ofast                # Aggressive optimization
        -march=native         # CPU-specific instructions
        -mtune=native         # Tune for current CPU
        -flto=auto            # Link-time optimization with parallel jobs
        -funroll-loops        # Unroll loops
        -fomit-frame-pointer  # Free up a register
        -fno-rtti             # Disable RTTI
    )
    
    if(COMPILER_SUPPORTS_BMI2)
        list(APPEND RELEASE_FLAGS -mbmi2)  # for pext instruction
        message(STATUS "Adding -mbmi2 flag in Release mode")
    else()
        message(STATUS "Compiler does not support -mbmi2; not adding it")
    endif()

    target_compile_options(chess PRIVATE ${RELEASE_FLAGS})
endif()

add_subdirectory(tests)