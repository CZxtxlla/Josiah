#ifndef BITS_H
#define BITS_H

#include "types.h"
#include <stdio.h>

#define A_FILE 0x0101010101010101ULL
#define B_FILE 0x0202020202020202ULL

#define G_FILE 0x4040404040404040ULL
#define H_FILE 0x8080808080808080ULL

#define RANK_8 0xFF00000000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_1 0x00000000000000FFULL

#define SetBit(board, square) ((board) |= (1ULL << (square)))
#define PopBit(board, square) ((board) &= ~(1ULL << (square)))
#define GetBit(board, square) ((board) & (1ULL << (square)))

#define FlipBit(board, square) ((board) ^= (1ULL << (square)))
#define FlipBits(board, sq1, sq2) ((board) ^= (1ULL << (sq1)) ^ (1ULL << (sq2)))

#define Rank(square) (square / 8)
#define File(square) (square % 8)

#define Piece(type, colour) (colour * 6 + type)

static inline int poplsb(Bitboard* board) {
    int lsb = __builtin_ctzll(*board);
    *board &= *board - 1;
    return lsb;
}

static inline void printBitboard(Bitboard board) {
    for (int i = 0; i < 64; i++) {
        if ((i & 7) == 0) {
            printf(" %d ", 8 - (i >> 3));
        }
        printf(" %d", GetBit(board, i ^ 56) ? 1 : 0);
        
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    printf("\n    a b c d e f g h\n\n");
}

#endif