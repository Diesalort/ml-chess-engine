add_executable(nn_tests
    tests_main.cpp)

target_link_libraries(nn_tests PRIVATE
    nn
    chess
    test_utils
)

add_test(NAME nn_tests COMMAND nn_tests)