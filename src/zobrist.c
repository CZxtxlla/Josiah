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

// get the hash of a position
uint64_t ZobristHash(Position* pos) {
    uint64_t hash = 0ULL;

    for (int pType = WHITE_PAWN; pType <= BLACK_KING; pType++) {
        Bitboard pieces = pos->pieces[pType];
        while (pieces) {
            hash ^= ZOBRIST_PIECES[pType][poplsb(&pieces)];
        }
    }

    if (pos->ep_square != -1) {
        hash ^= ZOBRIST_EP[pos->ep_square % 8];
    }

    hash ^= ZOBRIST_CASTLE[pos->castling];

    if (pos->stm) {
        hash ^= ZOBRIST_SIDE;
    }

    return hash;
}

// used for efficient prefetch
uint64_t nextHash(Position* pos, Move m) {
    if (!m) {
        return pos->hash ^ZOBRIST_SIDE;
    }

    int from = MoveFrom(m);
    int to = MoveTo(m);
    int movingType = pos->squares[from];

    uint64_t newHash = pos->hash ^ ZOBRIST_SIDE ^ ZOBRIST_PIECES[movingType][from] ^ ZOBRIST_PIECES[movingType][to];

    if (pos->squares[to] != PIECE_NONE) {
        newHash ^= ZOBRIST_PIECES[pos->squares[to]][to];
    }

    return newHash;
}