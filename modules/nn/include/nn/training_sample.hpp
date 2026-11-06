#pragma once

#include <cstdint>

#pragma pack(push, 1)

// TODO Stockfish evaluation for value_position (Phase 2)

struct TrainingSample
{
    /**
     * 0...5: Player's turn pieces (P, N, B, R, Q, K)
     * 6...11: Opponent's pieces (p, n, b, r, q, k)
     */
    uint64_t bitboards[12];

    /**
     * Castlings:
     * bit 0: Player's turn short castling
     * bit 1: Player's turn long castling
     * bit 2: Opponent's short castling
     * bit 3: Opponent's long castling
     */
    uint8_t castling;

    /**
     * En passant square:
     * 0...63 if there is
     * 64 if there is not 
     */
    uint8_t en_passant_square;

    // Fifty-move rule
    uint8_t rule50;

    /**
     * Side to move:
     * 0: white
     * 1: black
     */
    uint8_t side_to_move;

    // Label from 0...1857 for Policy head
    uint16_t best_move_idx;

    /**
     * Evaluation of the position for Value head.
     * Phase 1:
     *    - +1: Game favorable to player's turn
     *    - -1: Game favorable to opponent
     *    -  0: Game favorable to draw
     * Phase 2: Stockfish evaluation
     */
    int16_t value_position;
};

#pragma pack(pop)

static_assert(sizeof(TrainingSample) == 104, "The size of trainingSample struct must be 104 bytes.");