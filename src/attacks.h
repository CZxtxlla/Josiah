#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.h"

extern Bitboard PAWN_ATTACKS[2][64]; // colour, square
extern Bitboard KNIGHT_ATTACKS[64];
extern Bitboard KING_ATTACKS[64];

void initPawnAttacks();
void initKnightAttacks();
void initKingAttacks();

Bitboard getPawnAttacks(int colour, int square);
Bitboard getKnightAttacks(int square);
Bitboard getKingAttacks(int square);


#endif