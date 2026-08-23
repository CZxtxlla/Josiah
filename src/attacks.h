#ifndef ATTACKS_H
#define ATTACKS_H

#include "types.h"

extern Bitboard PAWN_ATTACKS[2][64]; // colour, square
extern Bitboard KNIGHT_ATTACKS[64];
extern Bitboard KING_ATTACKS[64];
extern Bitboard ROOK_ATTACKS[64][4096];
extern Bitboard BISHOP_ATTACKS[64][512];

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

int magicIndex(uint64_t magic, Bitboard blockers, int n);

void initBishopMasks();
void initRookMasks();

void initBishopMagics();
void initRookMagics();

void initBishopAttacks();
void initRookAttacks();

void initAttacks();

Bitboard getBishopAttacks(int square, Bitboard occupancy);
Bitboard getRookAttacks(int square, Bitboard occupancy);
Bitboard getQueenAttacks(int square, Bitboard occupancy);

void testBishopAttacks();
void testRookAttacks();

#endif