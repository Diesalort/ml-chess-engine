#pragma once

#include "chess/board.hpp"
#include "chess/move_generator.hpp"
#include "chess/move_list.hpp"
#include <string_view>

class SanParser
{
public:
    SanParser() = delete;

    /**
     * Convert a Standard Algebraic Notation (SAN) string (e.g. "Nf3", "Bxc1", ...) 
     * into the corresponding legal move in the current board.
     * Return an invalid Move if no legal move matches.
     */
    static Move san_to_move(Board& board, std::string_view san);

private:
};
