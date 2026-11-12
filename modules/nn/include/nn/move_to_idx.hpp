#pragma once

#include "chess/move.hpp"
#include <string_view>
#include <cstdint>
#include <cassert>

class MoveToIdx
{
public:
    /**
     * Convert a Move object to its corresponding index [0, 1857].
     */
    static inline int move_to_idx(Move move, bool black_to_move)
    {
        assert(move.is_valid());

        uint8_t sq_from = static_cast<uint8_t>(move.square_from().value());
        uint8_t sq_to = static_cast<uint8_t>(move.square_to().value());

        uint8_t promotion_type = 0;   // Nothing or promotion to Queen
        if (move.type() == MoveType::PROMOTION && move.promotion_piece() != PieceType::QUEEN) {
            // KNIGHT = 1; BISHOP = 2; ROOK = 3;
            promotion_type = static_cast<uint8_t>(move.promotion_piece());
            assert(promotion_type >= 1 && promotion_type <= 3);
        }

        return move_to_idx(sq_from, sq_to, promotion_type, black_to_move);
    }

    /**
     * Convert a move to its corresponding index [0, 1857].
     * The move is given by it's 'sq_from' and 'sq_to' int position.
     * 'promotion_type' can be:
     *    - 0: Queen or nothing
     *    - 1: Knight
     *    - 2: Bishop
     *    - 3: Rook
     * 
     * If 'black_to_move' is true, the vertical mirror (sq ^ 56) is applied to 
     * both from and to squares. 
     * The Vertical mirror swaps the ranks so the board is viewed from White's side,
     * leaving the files unchanged, so the position is always encoded from the side to move's
     * perspective.
     */
    static inline int move_to_idx(uint8_t sq_from, uint8_t sq_to, uint8_t promotion_type, bool black_to_move)
    {
        assert(sq_from < 64);
        assert(sq_to < 64 && sq_from != sq_to);
        assert(promotion_type <= 3);

        if (black_to_move) {
            sq_from ^= 56;
            sq_to ^= 56;
        }

        if (promotion_type == 0) {
            return tables_.normal_move_to_idx[sq_from][sq_to];
        }

        /**
         * Check the move is in fact a promotion and it's valid:
         *    - 'sq_from' is in file 6
         *    - 'sq_to' is in file 7
         *    - 'promotion_type' is in [1,3]
         */
        assert((sq_from / 8) == 6 && (sq_to / 8) == 7);
        return tables_.underpromotion_to_idx[sq_from % 8][sq_to % 8][promotion_type - 1];
    }

    /**
     * Overload for UCI strings.
     * Convert a UCI move (e.g. "g1f3") to its corresponding index [0, 1857].
     */
    static inline int uci_move_to_idx(std::string_view uci_move, bool black_to_move)
    {
        if (uci_move.size() < 4 || uci_move.size() > 5) return -1;

        if (uci_move[0] < 'a' || uci_move[0] > 'h' || uci_move[1] < '1' || uci_move[1] > '8' || uci_move[2] < 'a' ||
            uci_move[2] > 'h' || uci_move[3] < '1' || uci_move[3] > '8') {
            return -1;
        }

        uint8_t col_from = uci_move[0] - 'a';
        uint8_t row_from = uci_move[1] - '1';
        uint8_t col_to = uci_move[2] - 'a';
        uint8_t row_to = uci_move[3] - '1';

        uint8_t sq_from = row_from * 8 + col_from;
        uint8_t sq_to = row_to * 8 + col_to;

        if (sq_from == sq_to) return -1;

        uint8_t promotion_type = 0;   // Nothing or Queen
        if (uci_move.size() == 5) {   // There is a promotion
            uint8_t expected_row_from = black_to_move ? 1 : 6;
            uint8_t expected_row_to = black_to_move ? 0 : 7;
            if (row_from != expected_row_from || row_to != expected_row_to) {
                return -1;
            }

            switch (uci_move[4]) {
            case 'q': promotion_type = 0; break;
            case 'n': promotion_type = 1; break;
            case 'b': promotion_type = 2; break;
            case 'r': promotion_type = 3; break;
            default: return -1;
            }
        }

        return move_to_idx(sq_from, sq_to, promotion_type, black_to_move);
    }

private:
    struct LookUpTables
    {
        // normal_move_to_idx[sq_from][sq_to] (look-up table), excluding underpromotions
        int16_t normal_move_to_idx[64][64] {};

        /**
         * underpromotion_to_idx[sq_in_file_6 % 8][sq_in_file_7 % 8][promotion_type]
         * look-up table for underpromotions, where 'promotion_type' can be:
         *    - 0: Knight
         *    - 1: Bishop
         *    - 2: Rook 
         */
        int16_t underpromotion_to_idx[8][8][3] {};
    };

    static constexpr int abs(int x) { return x < 0 ? -x : x; }

    /**
     * For initializing the tables, we map each of the possible
     * movements from sq_from to sq_to to an index within the range [0, 1857].
     */
    static constexpr LookUpTables init_tables()
    {
        LookUpTables tables {};

        int idx = 0;   // index of the movements

        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                tables.normal_move_to_idx[i][j] = -1;
            }
        }

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 3; k++) {
                    tables.underpromotion_to_idx[i][j][k] = -1;
                }
            }
        }

        // Queen-type moves (this includes rook and bishop moves)
        for (int from = 0; from < 64; from++) {

            int row_from = from / 8;
            int col_from = from % 8;

            for (int to = 0; to < 64; to++) {
                if (from == to) continue;   // from and to square can't be the same

                int row_to = to / 8;
                int col_to = to % 8;

                // 0 if from and to share row; another value otherwise
                int row_dist = row_to - row_from;

                // 0 if from and to share col; another value otherwise
                int col_dist = col_to - col_from;

                // Queen move type: Same row, same col or diagonally. It includes bishop's and rook's move
                bool queen_move = !row_dist || !col_dist || abs(row_dist) == abs(col_dist);

                // Knight move: "L-shaped"
                bool knight_move =
                    (abs(row_dist) == 2 && abs(col_dist) == 1) || (abs(row_dist) == 1 && abs(col_dist) == 2);

                if (queen_move || knight_move) {

                    tables.normal_move_to_idx[from][to] = idx++;

                    // Check if there could be a promotion between from and to squares
                    if (row_from == 6 && row_to == 7 && abs(col_dist) <= 1) {

                        for (int k = 0; k <= 2; k++) {
                            tables.underpromotion_to_idx[col_from][col_to][k] = idx++;
                        }
                    }
                }
            }
        }

        assert(idx == 1858);

        return tables;
    }

    static const LookUpTables tables_;
};

inline constexpr MoveToIdx::LookUpTables MoveToIdx::tables_ = MoveToIdx::init_tables();