/*
 * 
 * ==============================
 *  MODIFICATION ADVERTISEMENT
 * ==============================
 * 
 * Code extracted from: https://github.com/LauraWangQiu/AlphaDeepChess
 * Original filename: testsAlphaDeepChess.cpp
 * 
 * MODIFIED BY: Diego Salort
 * DATE: 2026-10-12
 * 
 * - Renamed file from 'testsAlphaDeepChess.cpp' to 'tests_main.cpp'.
 * - Removed 'transposition_table_test.cpp': the Transposition Table is not used
 * in this project.
 * - Removed 'search_test.cpp': the Alpha-Beta search engine 
 * is not used (this project uses MCTS+ResNet instead).
 * 
 */

#include "move_generator_test.cpp"
#include "board_test.cpp"
#include "game_state_test.cpp"
#include "square_test.cpp"
#include "move_test.cpp"
#include "move_list_test.cpp"
#include "piece_test.cpp"
#include "coordinates_test.cpp"
#include "row_test.cpp"
#include "col_test.cpp"
#include "diagonal_test.cpp"
#include "zobrist_test.cpp"

int main()
{
    piece_test();
    row_test();
    col_test();
    coordinates_test();
    game_state_test();
    square_test();
    move_test();
    move_list_test();
    board_test();
    diagonal_test();
    zobrist_test();
    move_generator_test();

    return 0;
}