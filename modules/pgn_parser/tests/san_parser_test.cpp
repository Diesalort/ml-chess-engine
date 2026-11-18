#include "pgn_parser/san_parser.hpp"
#include "test_utils.hpp"

static void san_parser_normal_moves_test();
static void san_parser_castling_test();
static void san_parser_promotions_test();
static void san_parser_captures_test();
static void san_parser_disambiguation_test();
static void san_parser_annotations_and_invalid_test();


void san_parser_test()
{

    std::cout << "---------san_parser test---------\n\n";

    san_parser_normal_moves_test();
    san_parser_castling_test();
    san_parser_promotions_test();
    san_parser_captures_test();
    san_parser_disambiguation_test();
    san_parser_annotations_and_invalid_test();
}

static void san_parser_normal_moves_test()
{

    const std::string test_name = "san_parser_normal_moves_test";

    // Starting position FEN
    constexpr auto FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Board board;
    board.load_fen(FEN);

    // 1. white: e3 (pawn, one-square advance)
    Move e4 = SanParser::san_to_move(board, "e3");
    if (e4 != Move(Square::E2, Square::E3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'e3'");
    }
    board.make_move(e4);

    // 1. black: d5 (pawn, two-square advance)
    Move c5 = SanParser::san_to_move(board, "d5");
    if (c5 != Move(Square::D7, Square::D5)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'd5'");
    }
    board.make_move(c5);

    // 2. white: Nc3 (knight)
    Move nc3 = SanParser::san_to_move(board, "Nc3");
    if (nc3 != Move(Square::B1, Square::C3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'Nc3'");
    }
    board.make_move(nc3);

    // 2. black: Bh3 (bishop)
    Move bh3 = SanParser::san_to_move(board, "Bh3");
    if (bh3 != Move(Square::C8, Square::H3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'Bh3'");
    }
}

static void san_parser_castling_test()
{

    const std::string test_name = "san_parser_castling_test";

    // FEN with both black and white castlings available
    constexpr auto FEN = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";

    Board board;
    board.load_fen(FEN);

    // White's kingside castling (O-O)
    Move wkc = SanParser::san_to_move(board, "O-O");
    if (wkc != Move(Square::E1, Square::G1, MoveType::CASTLING)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'O-O' (White's kingside castling)");
    }

    // White's queenside castling (O-O-O)
    Move wqc = SanParser::san_to_move(board, "O-O-O");
    if (wqc != Move(Square::E1, Square::C1, MoveType::CASTLING)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'O-O-O' (White's queenside castling)");
    }

    // Normal move
    Move d4 = SanParser::san_to_move(board, "d4");
    if (d4 != Move(Square::D2, Square::D4)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'd4'");
    }
    board.make_move(d4);

    // Black's kingside castling
    Move bkc = SanParser::san_to_move(board, "O-O");
    if (bkc != Move(Square::E8, Square::G8, MoveType::CASTLING)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'O-O' (Black's kingside castling)");
    }

    // Black's queenside castling (O-O-O)
    Move bqc = SanParser::san_to_move(board, "O-O-O");
    if (bqc != Move(Square::E8, Square::C8, MoveType::CASTLING)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse 'O-O-O' (Black's queenside castling)");
    }
}

static void san_parser_promotions_test()
{

    const std::string test_name = "san_parser_promotions_test";

    // FEN with pawns on a7 and d7 ready to promote
    constexpr auto FEN = "4r2k/P2P4/8/8/8/8/8/K7 w - - 0 1";

    Board board;
    board.load_fen(FEN);

    // Promotion to a queen (a8=Q)
    Move a8q = SanParser::san_to_move(board, "a8=Q");
    if (a8q != Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::QUEEN)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse promotion 'a8=Q'");
    }

    // Underpromotion to a knight (a8=N)
    Move a8k = SanParser::san_to_move(board, "a8=N");
    if (a8k != Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::KNIGHT)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse promotion 'a8=N'");
    }

    // Underpromotion to a bishop (a8=B)
    Move a8b = SanParser::san_to_move(board, "a8=B");
    if (a8b != Move(Square::A7, Square::A8, MoveType::PROMOTION, PieceType::BISHOP)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse promotion 'a8=B'");
    }

    // Underpromotion to a rook with capture and check (dxe8=R+)
    Move dxe8r = SanParser::san_to_move(board, "dxe8=R+");
    if (dxe8r != Move(Square::D7, Square::E8, MoveType::PROMOTION, PieceType::ROOK)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse underpromotion with capture and check 'dxe8=R+'");
    }
}

static void san_parser_captures_test()
{

    const std::string test_name = "san_parser_captures_test";

    // FEN with possibility of en passant capture on d3
    constexpr auto FEN = "rnbqkbnr/pppp1ppp/8/8/3Pp3/N6P/PPP1PPP1/R1BQKBNR b KQkq d3 0 1";

    Board board;
    board.load_fen(FEN);

    // 1. black: En passant capture (exd3)
    Move exd3 = SanParser::san_to_move(board, "exd3");
    if (exd3 != Move(Square::E4, Square::D3, MoveType::EN_PASSANT)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse en passant capture 'exd3'");
    }
    board.make_move(exd3);

    // 1. white: Queen captures the pawn on d3 (Qxd3)
    Move qxd3 = SanParser::san_to_move(board, "Qxd3");
    if (qxd3 != Move(Square::D1, Square::D3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse queen capture 'Qxd3'");
    }
    board.make_move(qxd3);

    // 2. black: Bishop captures knight on a3 (Bxa3)
    Move bxa3 = SanParser::san_to_move(board, "Bxa3");
    if (bxa3 != Move(Square::F8, Square::A3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse bishop capture 'Bxa3'");
    }
    board.make_move(bxa3);

    // 2. white: Queen captures bishop on a3 (Qxa3)
    Move qxa3 = SanParser::san_to_move(board, "Qxa3");
    if (qxa3 != Move(Square::D3, Square::A3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse queen capture 'Qxa3'");
    }
}

static void san_parser_disambiguation_test()
{

    const std::string test_name = "san_parser_disambiguation_test";

    // 1. Disambiguation by file (column)
    // One of two possible white rooks on the same rank can capture a black pawn
    constexpr auto FEN_df = "4k3/8/8/8/R1p4R/8/8/4K3 w - - 0 1";

    Board board;
    board.load_fen(FEN_df);

    // Rook on h4 captures the pawn
    Move rhxc4 = SanParser::san_to_move(board, "Rhxc4");
    if (rhxc4 != Move(Square::H4, Square::C4)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse file disambiguation 'Rhxc4'");
    }

    // 2. Disambiguation by rank (row)
    // One of two possible black queens on the same file can capture a white knight
    constexpr auto FEN_dr = "q3k3/8/8/8/8/N7/4K3/q7 b - - 0 1";

    board.load_fen(FEN_dr);

    // Queen on a8 captures the knight
    Move q8xa3 = SanParser::san_to_move(board, "Q8xa3");
    if (q8xa3 != Move(Square::A8, Square::A3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse rank disambiguation 'Q8xa3'");
    }

    // 3. Disambiguation by file and rank
    // Three black queens, two of them on the same file and two on the same rank, can capture a white knight
    constexpr auto FEN_dfr = "4k3/8/1q3q2/8/1q1N4/8/4K3/8 b - - 0 1";

    board.load_fen(FEN_dfr);

    // Queen on b6 captures the knight
    Move qb6xd4 = SanParser::san_to_move(board, "Qb6xd4");
    if (qb6xd4 != Move(Square::B6, Square::D4)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse file and rank disambiguation 'Qb6xd4'");
    }
}

static void san_parser_annotations_and_invalid_test()
{

    const std::string test_name = "san_parser_annotations_and_invalid_test";

    // Starting position FEN
    constexpr auto FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Board board;
    board.load_fen(FEN);

    // Annotations must be ignored
    Move nf3_annot = SanParser::san_to_move(board, "Nf3?!");
    if (nf3_annot != Move(Square::G1, Square::F3)) {
        PRINT_TEST_FAILED(test_name, "Failed to parse san with annotations 'Nf3?!'");
    }

    // Invalid moves must return Move() (null move)
    if (SanParser::san_to_move(board, "").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Empty SAN string returned valid move");
    }

    if (SanParser::san_to_move(board, "b5").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Illegal pawn move 'b5' returned valid move");
    }

    if (SanParser::san_to_move(board, "Qc1").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Illegal queen move 'Qc1' returned valid move");
    }

    if (SanParser::san_to_move(board, "Nc4").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Illegal knight move 'Nc4' returned valid move");
    }

    if (SanParser::san_to_move(board, "i5").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Invalid SAN 'i5' returned valid move");
    }

    // Unresolved disambiguation
    board.load_fen("4k3/8/8/8/R1p4R/8/8/4K3 w - - 0 1");
    if (SanParser::san_to_move(board, "Rc4").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Ambiguous 'Rc4' returned valid move");
    }

    // FEN with a pawn that can capture another pawn
    board.load_fen("4k3/8/4p3/3P4/8/8/8/4K3 w - - 0 1");
    if (SanParser::san_to_move(board, "e6").is_valid()) {
        PRINT_TEST_FAILED(test_name, "Illegal capture 'e6' ('x' was not specified)");
    }

    board.load_fen("4k3/8/4p3/3P4/8/8/8/4K3 w - - 0 1");
    if (SanParser::san_to_move(board, "xe6").is_valid()) {
        PRINT_TEST_FAILED(test_name,
                          "Illegal pawn capture 'xe6' (file letter of the originating square was not included)");
    }
}