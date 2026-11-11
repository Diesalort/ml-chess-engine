#pragma once

#include "chess/board.hpp"
#include "nn/training_sample.hpp"
#include "nn/move_to_idx.hpp"

class Encoder
{
public:
    /**
     * Encode the Board 'board' state in a TrainingSample structure.
     */
    static inline TrainingSample encode_board(Board const& board, Move const& chosen_move, int16_t white_result)
    {
        assert(white_result >= -1 && white_result <= 1);

        TrainingSample ts {};

        const GameState& state = board.state();
        const bool black_turn = state.side_to_move() == ChessColor::BLACK;

        // 1. Piece bitboards: [0, 5]: Side to move pieces; [6,11]: Opponent pieces
        for (int i = 0; i < 6; i++) {

            const uint64_t white_piece_bb = board.get_bitboard_piece(static_cast<Piece>(i));
            const uint64_t black_piece_bb = board.get_bitboard_piece(static_cast<Piece>(i + 6));
            if (!black_turn) {
                // White's turn
                ts.bitboards[i] = white_piece_bb;
                ts.bitboards[i + 6] = black_piece_bb;
            }
            else {
                // Black's turn
                ts.bitboards[i] = flip_vertical(black_piece_bb);
                ts.bitboards[i + 6] = flip_vertical(white_piece_bb);
            }
        }

        // 2. Castlings relative to side to move
        uint8_t castling = 0U;
        if (!black_turn) {
            if (state.castle_king_white()) castling |= 1U << 0;
            if (state.castle_queen_white()) castling |= 1U << 1;
            if (state.castle_king_black()) castling |= 1U << 2;
            if (state.castle_queen_black()) castling |= 1U << 3;
        }
        else {
            if (state.castle_king_black()) castling |= 1U << 0;
            if (state.castle_queen_black()) castling |= 1U << 1;
            if (state.castle_king_white()) castling |= 1U << 2;
            if (state.castle_queen_white()) castling |= 1U << 3;
        }
        ts.castling = castling;

        // 3. En passant square (0..63 relative to side to move; 64 if none)
        Square ep_sq = state.en_passant_square();
        if (ep_sq.is_valid()) {
            uint8_t ep_sq_value = ep_sq.value();
            ts.en_passant_square = black_turn ? (ep_sq_value ^ 56U) : ep_sq_value;
        }
        else {
            ts.en_passant_square = 64U;
        }

        // 4. Fifty-move rule
        ts.rule50 = std::min<uint8_t>(state.fifty_move_rule_counter(), 100U);

        // 5. Side to move
        ts.side_to_move = black_turn ? 1U : 0U;

        // 6. Best move idx [0,1857] for Policy head
        int move_idx = MoveToIdx::move_to_idx(chosen_move, black_turn);
        assert(move_idx >= 0 && move_idx <= 1857);
        ts.best_move_idx = static_cast<uint16_t>(move_idx);

        // 7. Evaluation of the position for Value head
        ts.value_position = black_turn ? -white_result : white_result;

        return ts;
    }

private:
    /**
     * Flips the board vertically (rank 1 <-> 8).
     * The files (columns) are preserved.
     */
    static inline uint64_t flip_vertical(uint64_t bb) noexcept
    {
#if defined(_MSC_VER)
        return _byteswap_uint64(bb);
#else
        return __builtin_bswap64(bb);
#endif
    }
};