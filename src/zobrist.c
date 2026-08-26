#include "zobrist.h"
#include "random.h"

uint64_t ZOBRIST_PIECES[12][64]; // piece, square
uint64_t ZOBRIST_EP[8]; // ep_square
uint64_t ZOBRIST_CASTLE[16]; // castling rights
uint64_t ZOBRIST_SIDE;

void initZobrist() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            ZOBRIST_PIECES[i][j] = random_U64();
        }
    }

    for (int i = 0; i < 8; i++) {
        ZOBRIST_EP[i] = random_U64();
    }

    for (int i = 0; i < 16; i++) {
        ZOBRIST_CASTLE[i] = random_U64();
    }

    ZOBRIST_SIDE = random_U64();
}