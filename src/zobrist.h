#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "position.h"
#include "move.h"

extern uint64_t ZOBRIST_PIECES[12][64]; // piece, square
extern uint64_t ZOBRIST_EP[8]; // ep file
extern uint64_t ZOBRIST_CASTLE[16]; // castling rights
extern uint64_t ZOBRIST_SIDE;

void initZobrist();

#endif