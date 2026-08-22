#ifndef BITS_H
#define BITS_H

#include "types.h"

#define A_FILE 0x0101010101010101ULL
#define B_FILE 0x0202020202020202ULL

#define G_FILE 0x4040404040404040ULL
#define H_FILE 0x8080808080808080ULL

#define RANK_8 0xFF00000000000000ULL
#define RANK_7 0x00FF000000000000ULL

#define RANK_2 0x000000000000FF00ULL
#define RANK_1 0x00000000000000FFULL

void printBitboard(Bitboard board);

#define SetBit(board, square) ((board) |= (1ULL << (square)))
#define PopBit(board, square) ((board) &= ~(1ULL << (square)))
#define GetBit(board, square) ((board) & (1ULL << (square)))

#endif