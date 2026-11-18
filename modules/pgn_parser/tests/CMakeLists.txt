add_executable(pgn_parser_tests
    tests_main.cpp)

target_link_libraries(pgn_parser_tests PRIVATE
    pgn_parser
    chess
    test_utils
)

add_test(NAME pgn_parser_tests COMMAND pgn_parser_tests)