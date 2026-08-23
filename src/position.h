#ifndef POSITION_H
#define POSITION_H

#include "types.h"
#include <stdio.h>
#include "bits.h"

typedef struct {
    Bitboard occupancies[3]; // 0 = white, 1 = black, 2 = both
    Bitboard pieces[12]; // one bitboard for each piece

    int castling; // 1111 = KQkq, 1001 = Kq
    int stm; // 0 for white, 1 for black

    uint64_t hash; // zobrist hash
} Position;

// fill pos with info from fen
void parseFen(char* fen, Position* pos);

static inline void printPosition(Position* pos) {
    for (int i = 0; i < 64; i++) {
        if ((i & 7) == 0) {
            printf(" %d ", 8 - (i >> 3));
        }
        for (int p = WHITE_PAWN; p <= BLACK_KING; p++) {
            Bitboard board = pos->pieces[p];
            if (GetBit(board, i ^ 56)) {
                switch(p) {
                    case WHITE_PAWN:
                        printf(" P");
                        break;
                    case WHITE_KNIGHT:
                        printf(" N");
                        break;
                    case WHITE_BISHOP:
                        printf(" B");
                        break;
                    case WHITE_ROOK:
                        printf(" R");
                        break;
                    case WHITE_QUEEN:
                        printf(" Q");
                        break;
                    case WHITE_KING:
                        printf(" K");
                        break;
                    case BLACK_PAWN:
                        printf(" p");
                        break;
                    case BLACK_KNIGHT:
                        printf(" n");
                        break;
                    case BLACK_BISHOP:
                        printf(" b");
                        break;
                    case BLACK_ROOK:
                        printf(" r");
                        break;
                    case BLACK_QUEEN:
                        printf(" q");
                        break;
                    case BLACK_KING:
                        printf(" k");
                        break;
                    default:
                        printf(" Error");
                        break;
                }
                break;
            }
            
        }
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    printf("\n    a b c d e f g h\n\n");
}




#endif