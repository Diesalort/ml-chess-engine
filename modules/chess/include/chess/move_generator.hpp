#pragma once

/**
 * @file move_generator.hpp
 * @brief move generator services.
 *
 * chess move generator declarations. 
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
 * - Updated include paths for the new modular structure (e.g. "board.hpp" -> "chess/board.hpp").
 * 
 */

#include "chess/board.hpp"
#include "chess/move_list.hpp"

/**
 * @brief MoveGeneratorType
 *
 * Type of move generator.
 */
enum MoveGeneratorType
{
    ALL_MOVES,      // Generate all legal moves
    ONLY_CAPTURES   // Generate only capture moves
};

/**
 * @brief generate_legal_moves
 * 
 * Calculate all the legal moves in the chess position.
 * 
 * @param[out] moves move list.
 * @param[in] board chess position.
 * @param[out] inCheck (optional) return true if the king is in check.
 * 
 */
template<MoveGeneratorType genType>
void generate_legal_moves(MoveList& moves, Board& board, bool* inCheck = nullptr);
