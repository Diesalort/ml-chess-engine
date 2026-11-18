add_library(pgn_parser STATIC
   src/san_parser.cpp
)

target_include_directories(pgn_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(pgn_parser PUBLIC
   chess
   nn
)

add_subdirectory(tests)