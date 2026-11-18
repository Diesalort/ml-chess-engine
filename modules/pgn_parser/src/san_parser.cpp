#include "pgn_parser/san_parser.hpp"
#include "chess/move_generator.hpp"
#include "chess/move_list.hpp"

Move SanParser::san_to_move(Board& board, std::string_view san)
{
    // Remove '+' (check), '#' (checkmate), and additional annotations such as '?' (usually poor move) and '!' (usually good move)
    while (!san.empty() && (san.back() == '+' || san.back() == '#' || san.back() == '?' || san.back() == '!')) {
        san.remove_suffix(1);
    }

    if (san.empty()) {
        return Move();
    }

    // Kingside castling ("O-O")
    if (san == "O-O" || san == "0-0") {
        MoveList legal_moves;
        generate_legal_moves<ALL_MOVES>(legal_moves, board);
        for (const Move& move : legal_moves) {
            if (move.type() == MoveType::CASTLING && move.square_to().col() == COL_G) {
                return move;
            }
        }
        return Move();
    }

    // Queenside castling ("O-O-O")
    if (san == "O-O-O" || san == "0-0-0") {
        MoveList legal_moves;
        generate_legal_moves<ALL_MOVES>(legal_moves, board);
        for (const Move& move : legal_moves) {
            if (move.type() == MoveType::CASTLING && move.square_to().col() == COL_C) {
                return move;
            }
        }
        return Move();
    }

    // Promotions
    PieceType promo_piece = PieceType::EMPTY;
    if (san.size() >= 2 && san[san.size() - 2] == '=') {
        switch (san.back()) {
        case 'Q': promo_piece = PieceType::QUEEN; break;
        case 'N': promo_piece = PieceType::KNIGHT; break;
        case 'B': promo_piece = PieceType::BISHOP; break;
        case 'R': promo_piece = PieceType::ROOK; break;
        default: return Move();
        }
        san.remove_suffix(2);
    }

    // Destination square (last 2 characters)
    if (san.size() < 2) return Move();

    const char col_to_char = san[san.size() - 2];
    const char row_to_char = san[san.size() - 1];

    if (col_to_char < 'a' || col_to_char > 'h' || row_to_char < '1' || row_to_char > '8') {
        return Move();
    }

    const uint8_t col_to = static_cast<uint8_t>(col_to_char - 'a');
    const uint8_t row_to = static_cast<uint8_t>(row_to_char - '1');
    const Square sq_to(row_to * 8 + col_to);

    san.remove_suffix(2);

    // Moving PieceType (PAWN by default)
    PieceType moving_piece_type = PieceType::PAWN;

    if (!san.empty() && san.front() >= 'A' && san.front() <= 'Z') {
        switch (san.front()) {
        case 'N': moving_piece_type = PieceType::KNIGHT; break;
        case 'B': moving_piece_type = PieceType::BISHOP; break;
        case 'R': moving_piece_type = PieceType::ROOK; break;
        case 'Q': moving_piece_type = PieceType::QUEEN; break;
        case 'K': moving_piece_type = PieceType::KING; break;
        default: return Move();
        }
        san.remove_prefix(1);
    }

    // Capture indicator ('x')
    bool capture = false;
    if (!san.empty() && san.back() == 'x') {
        san.remove_suffix(1);
        capture = true;
    }

    // Disambiguation
    int disambig_col = -1;
    int disambig_row = -1;

    if (san.size() > 2) return Move();
    for (char c : san) {
        if (c >= 'a' && c <= 'h' && disambig_col == -1 && disambig_row == -1) disambig_col = c - 'a';
        else if (c >= '1' && c <= '8' && disambig_row == -1)
            disambig_row = c - '1';
        else
            return Move();
    }

    if (moving_piece_type == PieceType::PAWN && capture && disambig_col == -1) {
        return Move();
    }

    // Match with a legal move
    const ChessColor side_to_move = board.state().side_to_move();
    const Piece expected_piece = create_piece(moving_piece_type, side_to_move);

    MoveList legal_moves;
    if (capture) {
        generate_legal_moves<ONLY_CAPTURES>(legal_moves, board);
    }
    else {
        generate_legal_moves<ALL_MOVES>(legal_moves, board);
    }

    const uint64_t expected_bb = board.get_bitboard_piece(expected_piece);
    Move matched_move;
    for (const Move& move : legal_moves) {
        if (move.square_to() != sq_to) continue;

        // Verify if it is a capture
        const bool is_move_capture = !board.is_empty(sq_to) || move.type() == MoveType::EN_PASSANT;
        if (is_move_capture != capture) continue;


        if (promo_piece != PieceType::EMPTY) {
            if (move.type() != MoveType::PROMOTION || move.promotion_piece() != promo_piece) continue;
        }
        else if (move.type() == MoveType::PROMOTION)
            continue;

        const Square sq_from = move.square_from();
        if (disambig_col != -1 && static_cast<int>(sq_from.col()) != disambig_col) continue;
        if (disambig_row != -1 && static_cast<int>(sq_from.row()) != disambig_row) continue;

        if ((expected_bb & sq_from.mask()) != 0ULL) {

            // If we had already found a valid move, the notation is ambiguous
            if (matched_move.is_valid()) {
                return Move();
            }
            matched_move = move;
        }
    }

    return matched_move;
}