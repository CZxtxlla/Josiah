#include "attacks.h"
#include "bits.h"

#define A_FILE 0x0101010101010101ULL
#define H_FILE 0x8080808080808080ULL

Bitboard PAWN_ATTACKS[2][64];

void initPawnAttacks() {
    // fill up the PAWN_ATTACKS array
    for (int sq = 0; sq < 64; sq++) {
        Bitboard board = 0; 
        Bitboard white_attacks = 0;
        Bitboard black_attacks = 0;

        SetBit(board, sq);

        // shift pawns diagonally
        black_attacks |= (board >> 7) & ~A_FILE;
        black_attacks |= (board >> 9) & ~H_FILE;
        white_attacks |= (board << 7) & ~H_FILE;
        white_attacks |= (board << 9) & ~A_FILE; 

        PAWN_ATTACKS[WHITE][sq] = white_attacks;
        PAWN_ATTACKS[BLACK][sq] = black_attacks;
    }
}

Bitboard getPawnAttacks(int colour, int square) {
    return PAWN_ATTACKS[colour][square];
}