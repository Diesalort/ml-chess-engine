#pragma once

#include <cstdint>

struct TrainingSample
{
    /**
     * 0...5: Side to move's pieces (P, N, B, R, Q, K)
     * 6...11: Opponent's pieces (p, n, b, r, q, k)
     */
    uint64_t bitboards[12];

    /**
     * Castlings:
     * bit 0: Side to move's short castling
     * bit 1: Side to move's long castling
     * bit 2: Opponent's short castling
     * bit 3: Opponent's long castling
     */
    uint8_t castling;

    /**
     * En passant square:
     * 0...63 if there is
     * 64 if none 
     */
    uint8_t en_passant_square;

    // Fifty-move rule
    uint8_t rule50;

    /**
     * Final result of the match:
     *    - +1: Side to move wins
     *    - -1: Side to move loses
     *    -  0: Draw
     */
    int8_t game_result;

    // Best move encoded in the range [0, 1857] for Policy head
    uint16_t best_move_idx;

    // Side to move evaluation of the position in centipawns for Value head.
    int16_t stm_eval_cp;
};

static_assert(sizeof(TrainingSample) == 104, "TrainingSample struct size must be 104 bytes.");