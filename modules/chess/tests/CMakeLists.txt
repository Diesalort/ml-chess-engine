add_executable(chess_tests
    tests_main.cpp
)

target_link_libraries(chess_tests PRIVATE chess)

add_test(NAME chess_tests COMMAND chess_tests)
