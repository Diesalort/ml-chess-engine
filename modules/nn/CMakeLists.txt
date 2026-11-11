add_library(nn INTERFACE)

target_include_directories(nn INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(nn INTERFACE chess)

add_subdirectory(tests)