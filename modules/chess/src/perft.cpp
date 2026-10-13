/**
 * @file perft.cpp
 * @brief perft services.
 *
 * perft test types and utilities declaration.
 * 
 * https://www.chessprogramming.org/Perft
 * 
 */

/*
 * 
 * ==============================
 *  NOTICE OF MODIFICATION
 * ==============================
 * 
 * Code extracted from: https://github.com/LauraWangQiu/AlphaDeepChess
 * 
 * MODIFIED BY: Diego Salort
 * DATE: 2026-10-12
 * 
 * - Updated include paths for the new modular structure (e.g. "perft.hpp" -> "chess/perft.hpp").
 * - Removed dependencies on the search module (transposition_table.hpp, search_utils.hpp).
 * - Removed 'store_nodes_in_tt()' method.
 * - Removed 'get_nodes_in_tt()' method.
 * - Removed template parameter 'bool use_tt' from 'perft_recursive()' method and its associated code.
 * - Removed parameter 'bool use_tt' from 'perft()' method and its associated code.
 * 
 * The original perft used a Transposition Table designed for the alpha-beta search engine.
 * This project uses MCTS+ResNet instead, so the TT is not needed.
 * While removing the Transposition Table reduces the speed of the perft test (used only for validation),
 * it decouples the chess logic from the search module, keeping the library self-contained.
 */

#include "chess/perft.hpp"
#include "chess/board.hpp"
#include "chess/zobrist.hpp"
#include <chrono>

/**
 * @brief perft_recursive
 * 
 * Count all the nodes of the position until the maximum depth.
 * 
 * @param[in,out] board The output stream
 * @param[in] depth maximum depth to reach.
 * 
 * @return counted nodes.
 * 
 */
static uint64_t perft_recursive(Board& board, uint8_t depth);

/**
 * @brief perft
 * 
 * Debugging function to walk the move generation tree 
 * of strictly legal moves to count all the leaf nodes of a certain depth,
 * which can be compared to predetermined values and used to isolate bugs. In perft,
 * nodes are only counted at the end after the last makemove. Thus "higher" terminal nodes
 * (e.g. mate or stalemate) are not counted, instead the number of move paths of a certain depth.
 * 
 * @note Perft ignores draws by repetition, by the fifty-move rule and by insufficient material.
 * By recording the amount of time taken for each iteration, it's possible to compare the 
 * performance of different move generators or the same generator on different machines.
 * 
 * @param[in] FEN position to study.
 * @param[in] depth depth to reach.
 * @param[out] moveNodeList legal moves and their number of nodes
 * @param[out] time ms passed to complete the perft test.
 * 
 */
void perft(const std::string& FEN, uint64_t depth, MoveNodesList& moveNodeList, int64_t& time)
{
    Board board;
    board.load_fen(FEN);

    MoveList moves;
    moveNodeList.reserve(moves.size());

    generate_legal_moves<ALL_MOVES>(moves, board);

    for (int i = 0; i < moves.size(); i++) {
        moveNodeList.emplace_back(moves[i], 0ULL);
    }

    auto start = std::chrono::high_resolution_clock::now();


    const GameState game_state = board.state();

    for (int i = 0; i < moves.size(); i++) {
        board.make_move(moves[i]);
        uint64_t nodes = perft_recursive(board, depth - 1);
        moveNodeList[i].second = nodes;
        board.unmake_move(moves[i], game_state);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    time = duration.count();
}

/**
 * @brief perft_recursive
 * 
 * Count all the nodes of the position until the maximum depth.
 * 
 * @param[in,out] board The output stream
 * @param[in] depth maximum depth to reach.
 * 
 * @return counted nodes.
 * 
 */
static uint64_t perft_recursive(Board& board, uint8_t depth)
{
    assert(board.state().get_zobrist_key() == Zobrist::hash(board));

    if (depth == 0) {
        return 1ULL;
    }

    const GameState game_state = board.state();
    uint64_t nodes = 0ULL;

    MoveList moves;
    generate_legal_moves<ALL_MOVES>(moves, board);


    for (int i = 0; i < moves.size(); i++) {
        board.make_move(moves[i]);
        nodes += perft_recursive(board, depth - 1);
        board.unmake_move(moves[i], game_state);
    }

    return nodes;
}