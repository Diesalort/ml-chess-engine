#pragma once

#include "chess/board.hpp"
#include <string_view>

class SanParser
{
public:
    SanParser() = delete;

    /**
     * Converts a Standard Algebraic Notation (SAN) string (e.g. "Nf3", "Bxc1") 
     * into the corresponding legal move on the current board.
     * Returns an invalid Move if no legal move matches.
     */
    static Move san_to_move(Board& board, std::string_view san);
};
