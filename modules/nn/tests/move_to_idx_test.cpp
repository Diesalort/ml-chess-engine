#include <iostream>
#include "nn/move_to_idx.hpp"
#include "test_utils.hpp"

static void move_to_idx_normal_test();
static void move_to_idx_promotion_test();
static void move_to_idx_castling_test();
static void move_to_idx_symmetry_test();
static void move_to_idx_uci_test();

void move_to_idx_test()
{

    std::cout << "---------move_to_idx test---------\n\n";

    move_to_idx_normal_test();
    move_to_idx_promotion_test();
    move_to_idx_symmetry_test();
    move_to_idx_castling_test();
    move_to_idx_uci_test();
}

static void move_to_idx_normal_test()
{

    const std::string test_name = "move_to_idx_normal_test";

    // Tests from A1
    // A1 to B1, first registered move
    Move a1b1(Square::A1, Square::B1);
    if (MoveToIdx::move_to_idx(a1b1, false) != 0) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(A1, B1), false) != 0");
    }

    // A1 to H1, seventh registered move
    Move a1h1(Square::A1, Square::H1);
    if (MoveToIdx::move_to_idx(a1h1, false) != 6) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(A1, H1), false) != 6");
    }

    // A1 to A2, first vertical move
    Move a1a2(Square::A1, Square::A2);
    if (MoveToIdx::move_to_idx(a1a2, false) != 7) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(A1, A2), false) != 7");
    }

    // A1 to B2, first diagonal move
    Move a1b2(Square::A1, Square::B2);
    if (MoveToIdx::move_to_idx(a1b2, false) != 8) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(A1, B2), false) != 8");
    }

    // From A1 there are 23 valid movements [0-22], so B1 must start at index 23
    // B1 to A1, eighth registered move
    Move b1a1(Square::B1, Square::A1);
    if (MoveToIdx::move_to_idx(b1a1, false) != 23) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(B1, A1), false) != 23");
    }

    // Random possible moves
    Move c3b5(Square::C3, Square::B5);
    int idx_c3b5 = MoveToIdx::move_to_idx(c3b5, false);
    if (idx_c3b5 < 0 || idx_c3b5 >= 1858) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(C3, B5), false) out of range [0, 1857]");
    }

    Move f5c8(Square::F5, Square::C8);
    int idx_f5c8 = MoveToIdx::move_to_idx(f5c8, false);
    if (idx_f5c8 < 0 || idx_f5c8 >= 1858) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(Move(F5, C8), false) out of range [0, 1857]");
    }

    if (idx_c3b5 == idx_f5c8) {
        PRINT_TEST_FAILED(test_name,
                          "MoveToIdx::move_to_idx(Move(C3, B5), false) == MoveToIdx::move_to_idx(Move(F5, C8), false)");
    }

    // Invalid movement, must return -1
    Move a1b4(Square::A1, Square::B4);
    if (MoveToIdx::move_to_idx(a1b4, false) != -1) {
        PRINT_TEST_FAILED(test_name, "MoveToIdx::move_to_idx(a1b4, false) != -1");
    }
}

static void move_to_idx_promotion_test()
{

    const std::string test_name = "move_to_idx_promotion_test";

    Move promotion_queen = Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::QUEEN);
    Move promotion_knight = Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::KNIGHT);
    Move promotion_bishop = Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::BISHOP);
    Move promotion_rook = Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::ROOK);

    int idx_promo_queen = MoveToIdx::move_to_idx(promotion_queen, false);
    int idx_promo_knight = MoveToIdx::move_to_idx(promotion_knight, false);
    int idx_promo_bishop = MoveToIdx::move_to_idx(promotion_bishop, false);
    int idx_promo_rook = MoveToIdx::move_to_idx(promotion_rook, false);

    // All index must be within the range [0, 1857]
    if (idx_promo_queen < 0 || idx_promo_queen >= 1858) {
        PRINT_TEST_FAILED(test_name, "Move(A7, A8) with queen promotion out of bounds");
    }

    if (idx_promo_knight < 0 || idx_promo_knight >= 1858) {
        PRINT_TEST_FAILED(test_name, "Move(A7, A8) with knight promotion out of bounds");
    }

    if (idx_promo_bishop < 0 || idx_promo_bishop >= 1858) {
        PRINT_TEST_FAILED(test_name, "Move(A7, A8) with bishop promotion out of bounds");
    }

    if (idx_promo_rook < 0 || idx_promo_rook >= 1858) {
        PRINT_TEST_FAILED(test_name, "Move(A7, A8) with rook promotion out of bounds");
    }

    // promotion_queen must have the same index than a normal move
    Move normal_move = Move(Square::A7, Square::A8, MoveType::NORMAL);
    int idx_normal_move = MoveToIdx::move_to_idx(normal_move, false);
    if (idx_promo_queen != idx_normal_move) {
        PRINT_TEST_FAILED(test_name, "Move(A7, A8) with and without promotion to queen has different index");
    }

    // All the promotion index must be different
    if (idx_promo_queen == idx_promo_knight || idx_promo_queen == idx_promo_bishop ||
        idx_promo_queen == idx_promo_rook || idx_promo_knight == idx_promo_bishop ||
        idx_promo_knight == idx_promo_rook || idx_promo_bishop == idx_promo_rook) {
        PRINT_TEST_FAILED(test_name, "The indices for different promotions are not different");
    }

    // Symmetry in promotions of white and black pieces must have the same index
    Move white_promotion(Square::D7, Square::E8, MoveType::PROMOTION, PieceType::ROOK);
    Move black_promotion(Square::D2, Square::E1, MoveType::PROMOTION, PieceType::ROOK);

    int idx_white_promotion = MoveToIdx::move_to_idx(white_promotion, false);
    int idx_black_promotion = MoveToIdx::move_to_idx(black_promotion, true);
    if (idx_white_promotion != idx_black_promotion) {
        PRINT_TEST_FAILED(test_name, "Symmetry between black and white promotion failed");
    }
}

static void move_to_idx_castling_test()
{

    const std::string test_name = "move_to_idx_castling_test";

    int wk_castling_idx = MoveToIdx::move_to_idx(Move::castle_white_king(), false);
    if (wk_castling_idx < 0 || wk_castling_idx >= 1858) {
        PRINT_TEST_FAILED(test_name, "Move::castle_white_king() out of range [0, 1857]");
    }

    int wq_castling_idx = MoveToIdx::move_to_idx(Move::castle_white_queen(), false);
    if (wq_castling_idx < 0 || wq_castling_idx >= 1858) {
        PRINT_TEST_FAILED(test_name, "Move::castle_white_queen() out of range [0, 1857]");
    }

    // castle_black_king() must have same idx than castle_white_king because of symmetry
    int bk_castling_idx = MoveToIdx::move_to_idx(Move::castle_black_king(), true);
    if (wk_castling_idx != bk_castling_idx) {
        PRINT_TEST_FAILED(test_name, "Move::castle_white_king() != Move::castle_black_king()");
    }
}

static void move_to_idx_symmetry_test()
{

    const std::string test_name = "move_to_idx_symmetry_test";

    Move b1b3 = Move(Square::B1, Square::B3);   // White's move
    Move b8b6 = Move(Square::B8, Square::B6);   // Black's move
    int idx_b1b3 = MoveToIdx::move_to_idx(b1b3, false);
    int idx_b8b6 = MoveToIdx::move_to_idx(b8b6, true);
    if (idx_b1b3 != idx_b8b6) {
        PRINT_TEST_FAILED(test_name, "Symmetry failed: idx Move(B1, B3) != idx Move(B8, B6)");
    }

    Move b1c3 = Move(Square::B1, Square::C3);   // White's move
    Move b8c6 = Move(Square::B8, Square::C6);   // Black's move
    int idx_b1c3 = MoveToIdx::move_to_idx(b1c3, false);
    int idx_b8c6 = MoveToIdx::move_to_idx(b8c6, true);
    if (idx_b1c3 != idx_b8c6) {
        PRINT_TEST_FAILED(test_name, "Symmetry failed: idx Move(B1, C3) != idx Move(B8, C6)");
    }

    Move white_move_ep = Move(Square::D5, Square::E6, MoveType::EN_PASSANT);
    Move black_move_ep = Move(Square::D4, Square::E3, MoveType::EN_PASSANT);
    int idx_white_move_ep = MoveToIdx::move_to_idx(white_move_ep, false);
    int idx_black_move_ep = MoveToIdx::move_to_idx(black_move_ep, true);
    if (idx_white_move_ep != idx_black_move_ep) {
        PRINT_TEST_FAILED(test_name, "Symmetry failed (en passant): idx Move(D5, E6) != idx Move(D4, E3)");
    }
}

static void move_to_idx_uci_test()
{

    const std::string test_name = "move_to_idx_uci_test";

    Move f4g5 = Move(Square::F4, Square::G5);
    if (MoveToIdx::move_to_idx(f4g5, false) != MoveToIdx::uci_move_to_idx("f4g5", false)) {
        PRINT_TEST_FAILED(test_name,
                          "MoveToIdx::move_to_idx(f4g5, false) != MoveToIdx::uci_move_to_idx(\"f4g5\", false)");
    }

    Move a7a8q = Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::QUEEN);
    if (MoveToIdx::move_to_idx(a7a8q, false) != MoveToIdx::uci_move_to_idx("a7a8q", false)) {
        PRINT_TEST_FAILED(test_name,
                          "MoveToIdx::move_to_idx(a7a8q, false) != MoveToIdx::uci_move_to_idx(\"a7a8q\", false)");
    }

    Move h7g8r = Move(Square::H7, Square::G8, MoveType::PROMOTION, PieceType::ROOK);
    if (MoveToIdx::move_to_idx(h7g8r, false) != MoveToIdx::uci_move_to_idx("h7g8r", false)) {
        PRINT_TEST_FAILED(test_name,
                          "MoveToIdx::move_to_idx(h7g8r, false) != MoveToIdx::uci_move_to_idx(\"h7g8r\", false)");
    }

    Move d8e6 = Move(Square::D8, Square::E6);
    if (MoveToIdx::move_to_idx(d8e6, true) != MoveToIdx::uci_move_to_idx("d8e6", true)) {
        PRINT_TEST_FAILED(test_name,
                          "MoveToIdx::move_to_idx(d8e6, true) != MoveToIdx::uci_move_to_idx(\"d8e6\", true)");
    }
}