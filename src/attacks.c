#include "attacks.h"
#include "bits.h"
#include "random.h"
#include <string.h>
#include <stdio.h>

Bitboard PAWN_ATTACKS[2][64];
Bitboard KNIGHT_ATTACKS[64];
Bitboard KING_ATTACKS[64];

Bitboard ROOK_ATTACKS[64][4096];
Bitboard BISHOP_ATTACKS[64][512];

Bitboard BISHOP_MASKS[64];
Bitboard ROOK_MASKS[64];

uint64_t BISHOP_MAGICS[64];
uint64_t ROOK_MAGICS[64];

const int BISHOP_BITS[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

const int ROOK_BITS[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

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

void initBishopMasks() {
    for (int sq = 0; sq < 64; sq++) {
        Bitboard attacks = 0;

        int rank = Rank(sq);
        int file = File(sq);

        for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++) {
            SetBit(attacks, r * 8 + f);
        }
        for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--) {
            SetBit(attacks, r * 8 + f);
        }
        for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++) {
            SetBit(attacks, r * 8 + f);
        }
        for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--) {
            SetBit(attacks, r * 8 + f);
        }

        BISHOP_MASKS[sq] = attacks;
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

        ROOK_MASKS[sq] = attacks;
    }
}

Bitboard bishopAttacksCast(int square, Bitboard occupancy) {
    Bitboard attacks = 0;

    int rank = Rank(square);
    int file = File(square);

    for (int r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (GetBit(occupancy, r * 8 + f)) {
            break;
        }
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (GetBit(occupancy, r * 8 + f)) {
            break;
        }
    }
    for (int r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (GetBit(occupancy, r * 8 + f)) {
            break;
        }
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (GetBit(occupancy, r * 8 + f)) {
            break;
        }
    }

    return attacks;
}

Bitboard rookAttacksCast(int square, Bitboard occupancy) {
    Bitboard attacks = 0;

    int rank = Rank(square);
    int file = File(square);

    for (int r = rank + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + file));
        if (GetBit(occupancy, r * 8 + file)) {
            break;
        }
    }
    for (int r = rank - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + file));
        if (GetBit(occupancy, r * 8 + file)) {
            break;
        }
    }
    for (int f = file + 1; f <= 7; f++) {
        attacks |= (1ULL << (rank * 8 + f));
        if (GetBit(occupancy, rank * 8 + f)) {
            break;
        }
    }
    for (int f = file - 1; f >= 0; f--) {
        attacks |= (1ULL << (rank * 8 + f));
        if (GetBit(occupancy, rank * 8 + f)) {
            break;
        }
    }

    return attacks;
}

Bitboard setOccupancy(int index, int index_bits, Bitboard mask) {
    // get occupancy subset pertaining to the index
    Bitboard occupancy = 0;

    for (int i = 0; i < index_bits; i++) {
        int sq = __builtin_ctzll(mask);
        PopBit(mask, sq);

        if (index & (1 << i)) {
            occupancy |= (1ULL << sq);
        }
    }

    return occupancy;
}

uint64_t findMagic(int square, int index_bits, int is_bishop) {
    Bitboard mask = is_bishop ? BISHOP_MASKS[square] : ROOK_MASKS[square];

    Bitboard occupancies[4096];
    Bitboard attacks[4096];
    Bitboard used_attacks[4096];
    
    int subsets = 1ULL << index_bits;
    for (int i = 0; i < subsets; i++) {
        occupancies[i] = setOccupancy(i, index_bits, mask);
        attacks[i] = is_bishop ? bishopAttacksCast(square, occupancies[i]) : rookAttacksCast(square, occupancies[i]);
    }

    for (int i = 0; i < 10000000; i++) {
        uint64_t candidate = randomMagic();

        if (__builtin_popcountll((mask * candidate) & 0xFF00000000000000) < 6) {
            continue;
        }

        memset(used_attacks, 0, sizeof(used_attacks));

        int failed = 0;
        for (int j = 0; !failed && j < subsets; i++) {
            int index = (occupancies[j] * candidate) >> (64 - index_bits);

            if (!used_attacks[index]) {
                used_attacks[index] = attacks[j];
            } else if (used_attacks[index] != attacks[j]) {
                failed = 1;
            }
        }

        if (!failed) {
            return candidate;
        }
    }

    printf("Unable to find magic number.\n");
    return 0ULL;
}

void initBishopMagics() {
    // fill magic table
    for (int sq = 0; sq < 64; sq++) {
        BISHOP_MAGICS[sq] = findMagic(sq, BISHOP_BITS[sq], 1);
    }
}


void initRookMagics() {
    for (int sq = 0; sq < 64; sq++) {
        ROOK_MAGICS[sq] = findMagic(sq, ROOK_BITS[sq], 0);
    }
}

int magicIndex(uint64_t magic, Bitboard blockers, int n){
    return (blockers * magic) >> (64 - n);
}

void initBishopAttacks() {
    // fill attacks table
    for (int sq = 0; sq < 64; sq++) {
        int bits = BISHOP_BITS[sq];
        for (uint64_t i = 0; i < (1ULL << bits); i++) {
            Bitboard occupancy = setOccupancy(i, bits, BISHOP_MASKS[sq]);
            BISHOP_ATTACKS[sq][magicIndex(BISHOP_MAGICS[sq], occupancy, bits)] = bishopAttacksCast(sq, occupancy);
        }
    }
}

void initRookAttacks() {
    for (int sq = 0; sq < 64; sq++) {
        int bits = ROOK_BITS[sq];
        for (uint64_t i = 0; i < (1ULL << bits); i++) {
            Bitboard occupancy = setOccupancy(i, bits, ROOK_MASKS[sq]);
            ROOK_ATTACKS[sq][magicIndex(ROOK_MAGICS[sq], occupancy, bits)] = rookAttacksCast(sq, occupancy);
        }
    }
}

void initAttacks() {
    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();

    initBishopMasks();
    initRookMasks();

    initBishopMagics();
    initRookMagics();
    
    initBishopAttacks();
    initRookAttacks();
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

Bitboard getBishopAttacks(int square, Bitboard occupancy) {
    Bitboard blockers = occupancy & BISHOP_MASKS[square];
    uint64_t magic = BISHOP_MAGICS[square];

    return BISHOP_ATTACKS[square][magicIndex(magic, blockers, BISHOP_BITS[square])];
}

Bitboard getRookAttacks(int square, Bitboard occupancy) {
    Bitboard blockers = occupancy & ROOK_MASKS[square];
    uint64_t magic = ROOK_MAGICS[square];

    return ROOK_ATTACKS[square][magicIndex(magic, blockers, ROOK_BITS[square])];
}

Bitboard getQueenAttacks(int square, Bitboard occupancy) {
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}