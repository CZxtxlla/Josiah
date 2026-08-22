#ifndef BITS_H
#define BITS_H

#include "types.h"

void printBitboard(Bitboard board);

#define SetBit(board, square) ((board) |= (1ULL << (square)))
#define PopBit(board, square) ((board) &= ~(1ULL << (square)))
#define GetBit(board, square) ((board) & (1ULL << (square)))

#endif