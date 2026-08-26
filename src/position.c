#include "position.h"
#include <string.h>

const int CHAR_TO_PIECE[] = {
    ['P'] = WHITE_PAWN,
    ['N'] = WHITE_KNIGHT,
    ['B'] = WHITE_BISHOP,
    ['R'] = WHITE_ROOK,
    ['Q'] = WHITE_QUEEN,
    ['K'] = WHITE_KING,
    ['p'] = BLACK_PAWN,
    ['n'] = BLACK_KNIGHT,
    ['b'] = BLACK_BISHOP,
    ['r'] = BLACK_ROOK,
    ['q'] = BLACK_QUEEN,
    ['k'] = BLACK_KING
};

const char PIECE_TO_CHAR[] = {
    [WHITE_PAWN] = 'P',
    [WHITE_KNIGHT] = 'N',
    [WHITE_BISHOP] = 'B',
    [WHITE_ROOK] = 'R',
    [WHITE_QUEEN] = 'Q',
    [WHITE_KING] = 'K',
    [BLACK_PAWN] = 'p',
    [BLACK_KNIGHT] = 'n',
    [BLACK_BISHOP] = 'b',
    [BLACK_ROOK] = 'r',
    [BLACK_QUEEN] = 'q',
    [BLACK_KING] = 'k'
};

void parseFen(char* fen, Position* pos) {
    memset(pos, 0, sizeof(Position));

    for (int i = 0; i < 64; i++) {
        pos->squares[i] = PIECE_NONE;
    }

    int square = 56;
    // pieces
    while (*fen != ' ') {
        if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
            int piece = CHAR_TO_PIECE[(int)(*fen)];
            SetBit(pos->pieces[piece], square);
            pos->squares[square] = piece;
            square++;
        } else if (*fen >= '1' && *fen <= '8') {
            square += (*fen) - '0';
        } else if (*fen == '/') {
            square -= 16;
        }
        fen++;
    }

    fen++;

    // stm
    if (*fen == 'w') {
        pos->stm = WHITE;
        pos->xstm = BLACK;
        fen++;
    } else if (*fen == 'b') {
        pos->stm = BLACK;
        pos->xstm = WHITE;
        fen++;
    } else {
        printf("Invalid stm.\n");
    }

    fen++;

    // castling
    pos->castling = 0;
    while (*fen != ' ') {
        switch(*fen) {
            case 'K': pos->castling |= WHITE_KS; break;
            case 'Q': pos->castling |= WHITE_QS; break;
            case 'k': pos->castling |= BLACK_KS; break;
            case 'q': pos->castling |= BLACK_QS; break;
        }
        fen++;
    }

    fen++;

    // en passant square

    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = fen[1] - '1';

        pos->ep_square = rank * 8 + file;
        fen += 2;
    } else {
        pos->ep_square = -1;
        fen++;
    }

    while (*fen && *fen != ' ') {
        fen++;
    }
    
    pos->half_moves = 0;
    pos->full_moves = 0;
    sscanf(fen, " %d %d", &pos->half_moves, &pos->full_moves);

    // occupancies

    for (int p = WHITE_PAWN; p <= WHITE_KING; p++) {
        pos->occupancies[WHITE] |= pos->pieces[p];
        pos->occupancies[BOTH] |= pos->pieces[p];
    }

    for (int p = BLACK_PAWN; p <= BLACK_KING; p++) {
        pos->occupancies[BLACK] |= pos->pieces[p];
        pos->occupancies[BOTH] |= pos->pieces[p];
    }
}