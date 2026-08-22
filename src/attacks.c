#include "attacks.h"

void initPawnAttacks() {
    // fill up the PAWN_ATTACKS array
    for (int i = 0; i < 64; i++) {
        Bitboard board = 0; 
        Bitboard white_attacks = 0;
        Bitboard black_attacks = 0;

        SetBit(board, i);

        // shift pawns diagonally
        black_attacks |= 
        white_attacks |=

        PAWN_ATTACKS[WHITE][i] = white_attacks;
        PAWN_ATTACKS[BLACK][i] = black_attacks;

    }

}

Bitboard getPawnAttacks(int colour, int square) {
    return PAWN_ATTACKS[colour][square];
}