#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.h"

extern Bitboard PAWN_ATTACKS[2][64]; // colour, square
extern Bitboard KNIGHT_ATTACKS[64];
extern Bitboard KING_ATTACKS[64];

extern Bitboard BISHOP_MASKS[64];
extern Bitboard ROOK_MASKS[64];

extern uint64_t BISHOP_MAGICS[64];
extern uint64_t ROOK_MAGICS[64];

void initPawnAttacks();
void initKnightAttacks();
void initKingAttacks();

Bitboard getPawnAttacks(int colour, int square);
Bitboard getKnightAttacks(int square);
Bitboard getKingAttacks(int square);

void initBishopMasks();
void initRookMasks();

void initBishopMagics();
void initRookMagics();

Bitboard getBishopAttacks(int square, Bitboard blockers);
Bitboard getRookAttacks(int square, Bitboard blockers);




#endif