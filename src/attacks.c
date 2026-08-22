#include "attacks.h"
#include "bits.h"

Bitboard PAWN_ATTACKS[2][64];
Bitboard KNIGHT_ATTACKS[64];
Bitboard KING_ATTACKS[64];

void initPawnAttacks() {
    // fill up the PAWN_ATTACKS array
    for (int sq = 0; sq < 64; sq++) {
        Bitboard board = 0; 
        Bitboard white_attacks = 0;
        Bitboard black_attacks = 0;

        SetBit(board, sq);

        // shift pawns diagonally
        black_attacks |= ((board & ~H_FILE) >> 7);
        black_attacks |= ((board & ~A_FILE) >> 9);
        white_attacks |= ((board & ~A_FILE) << 7);
        white_attacks |= ((board & ~H_FILE) << 9);

        PAWN_ATTACKS[WHITE][sq] = white_attacks;
        PAWN_ATTACKS[BLACK][sq] = black_attacks;
    }
}

void initKnightAttacks() {
    // fill up the KNIGHT_ATTACKS array
    for (int sq = 0; sq < 64; sq++) {
        Bitboard board = 0; 
        Bitboard attacks = 0;

        SetBit(board, sq);

        // up
        if ((board << 6) & ~(G_FILE | H_FILE)) {
            attacks |= (board << 6);
        }
        if ((board << 10) & ~(A_FILE | B_FILE)) {
            attacks |= (board << 10);
        }
        if ((board << 15) & ~H_FILE) {
            attacks |= (board << 15);
        }
        if ((board << 17) & ~A_FILE) {
            attacks |= (board << 17);
        }

        // down
        if ((board >> 6) & ~(A_FILE | B_FILE)) {
            attacks |= (board >> 6);
        }
        if ((board >> 10) & ~(G_FILE | H_FILE)) {
            attacks |= (board >> 10);
        }
        if ((board >> 15) & ~A_FILE) {
            attacks |= (board >> 15);
        }
        if ((board >> 17) & ~H_FILE) {
            attacks |= (board >> 17);
        }

        KNIGHT_ATTACKS[sq] = attacks;
    }
}

void initKingAttacks() {
    // fill up the KING_ATTACKS array
    for (int sq = 0; sq < 64; sq++) {
        Bitboard board = 0; 
        Bitboard attacks = 0;

        SetBit(board, sq);

        // diagonals
        attacks |= ((board & ~H_FILE) >> 7);
        attacks |= ((board & ~A_FILE) >> 9);
        attacks |= ((board & ~A_FILE) << 7);
        attacks |= ((board & ~H_FILE) << 9);

        // vertical
        attacks |= (board >> 8);
        attacks |= (board << 8);

        // horizontal
        attacks |= ((board & ~H_FILE) << 1);
        attacks |= ((board & ~A_FILE) >> 1);

        KING_ATTACKS[sq] = attacks;
    }
}

Bitboard getPawnAttacks(int colour, int square) {
    return PAWN_ATTACKS[colour][square];
}

Bitboard getKnightAttacks(int square) {
    return KNIGHT_ATTACKS[square];
}

Bitboard getKingAttacks(int square) {
    return KING_ATTACKS[square];
}

void initBishopMasks() {
    for (int sq = 0; sq < 64; sq++) {
        Bitboard attacks = 0;

        int rank = Rank(sq);
        int file = File(sq);

        for (int r = rank + 1, f = file + 1; r < 7, f < 7; r++, f++) {
            SetBit(attacks, r * 8 + f);
        }
        for (int r = rank + 1, f = file - 1; r < 7, f > 0; r++, f--) {
            SetBit(attacks, r * 8 + f);
        }
        for (int r = rank - 1, f = file + 1; r > 0, f < 7; r--, f++) {
            SetBit(attacks, r * 8 + f);
        }
        for (int r = rank - 1, f = file - 1; r > 0, f > 0; r--, f--) {
            SetBit(attacks, r * 8 + f);
        }
    }
}

void initRookMasks() {
    for (int sq = 0; sq < 64; sq++) {
        Bitboard attacks = 0;

        int rank = Rank(sq);
        int file = File(sq);

        for (int r = rank + 1; r < 7; r++) {
            SetBit(attacks, r * 8 + file);
        }
        for (int r = rank - 1; r > 0; r--) {
            SetBit(attacks, r * 8 + file);
        }
        for (int f = file + 1; f < 7; f++) {
            SetBit(attacks, rank * 8 + f);
        }
        for (int f = file - 1; f > 0; f--) {
            SetBit(attacks, rank * 8 + f);
        }
    }
}