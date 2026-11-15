#include "test_utils.hpp"
#include "chess/board.hpp"
#include "nn/encoder.hpp"

static void encoder_symmetry_test();

void encoder_test()
{

    std::cout << "---------encoder test---------\n\n";

    encoder_symmetry_test();
}

static void encoder_symmetry_test()
{

    const std::string test_name = "encoder_symmetry_test";

    constexpr auto StartFEN_W = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    constexpr auto StartFEN_B = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";

    Board board_w;
    board_w.load_fen(StartFEN_W);
    Move move_w(Square::E2, Square::E4);

    Board board_b;
    board_b.load_fen(StartFEN_B);
    Move move_b(Square::E7, Square::E5);

    TrainingSample ts_w = Encoder::encode_board(board_w, move_w, 1, 35);
    TrainingSample ts_b = Encoder::encode_board(board_b, move_b, -1, 35);

    // 1. Bitboards
    for (int i = 0; i < 12; i++) {
        if (ts_w.bitboards[i] != ts_b.bitboards[i]) {
            PRINT_TEST_FAILED(test_name,
                              "bitboards[" + std::to_string(i) + "] doesn't match between White and Black start pos");
        }
    }

    // 2. Castlings
    if (ts_w.castling != 0b1111 || ts_b.castling != 0b1111) {
        PRINT_TEST_FAILED(test_name, "castling != 0b1111");
    }

    // 3. En passant square
    if (ts_w.en_passant_square != 64 || ts_b.en_passant_square != 64) {
        PRINT_TEST_FAILED(test_name, "en_passant_square != 64");
    }

    // 4. Fifty-move rule
    if (ts_w.rule50 != 0 || ts_b.rule50 != 0) {
        PRINT_TEST_FAILED(test_name, "rule50 != 0");
    }

    // 5. Game Result
    if (ts_w.game_result != 1 || ts_b.game_result != 1) {
        PRINT_TEST_FAILED(test_name, "Game result is wrong");
    }

    // 6. best_move_idx
    if (ts_w.best_move_idx != ts_b.best_move_idx) {
        PRINT_TEST_FAILED(test_name, "best_move_idx mismatch between White and Black");
    }

    // 7. Evaluation of the positions in centipawns
    if (ts_w.stm_eval_cp != 35 || ts_b.stm_eval_cp != 35) {
        PRINT_TEST_FAILED(test_name, "stm_eval_cp != 35");
    }
}