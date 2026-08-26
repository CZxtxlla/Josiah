#ifndef POSITION_H
#define POSITION_H

#include "types.h"
#include <stdio.h>
#include "bits.h"

extern const int CHAR_TO_PIECE[];
extern const char PIECE_TO_CHAR[];

#define WHITE_KS 0x1
#define WHITE_QS 0x2
#define BLACK_KS 0x4
#define BLACK_QS 0x8

typedef struct {
    Bitboard occupancies[3]; // 0 = white, 1 = black, 2 = both
    Bitboard pieces[12]; // one bitboard for each piece
    uint8_t squares[64]; 

    int castling; // 1111 = KQkq, 1001 = Kq
    int stm; // 0 for white, 1 for black
    int xstm;
    int ep_square;
    int half_moves;
    int full_moves;

    uint64_t hash; // zobrist hash
} Position;

typedef struct {
    int capture_piece;
    int castling;
    int ep_square;
    int movedPiece;
    int half_moves;

    uint64_t hash; 
} Undo;

// fill pos with info from fen
void parseFen(char* fen, Position* pos);

static inline void printPosition(Position* pos) {
    for (int i = 0; i < 64; i++) {
        if ((i & 7) == 0) {
            printf(" %d ", 8 - (i >> 3));
        }

        int found = 0;

        for (int p = WHITE_PAWN; p <= BLACK_KING; p++) {
            if (GetBit(pos->pieces[p], i ^ 56)) {
                found = 1;
                printf(" %c", PIECE_TO_CHAR[p]);
                break;
            }
        }
        if(!found) {
            printf(" .");
        }
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    printf("\n    a b c d e f g h\n\n");
}


#endif