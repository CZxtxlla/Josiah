#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.h"

extern Bitboard PAWN_ATTACKS[2][64]; // colour, square

void initPawnAttacks();

Bitboard getPawnAttacks(int colour, int square);


#endif