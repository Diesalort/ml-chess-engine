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

target_compile_options(chess PRIVATE -fno-rtti) # Disable RTTI

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if(COMPILER_SUPPORTS_BMI2)
        target_compile_options(chess PRIVATE -mbmi2) # for pext instruction
        message(STATUS "Adding -mbmi2 flag in Release mode")
    else()
        message(STATUS "Compiler does not support -mbmi2; not adding it")
    endif()
endif()

target_include_directories(chess PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)