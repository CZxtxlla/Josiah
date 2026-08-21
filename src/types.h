#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#define MAX_MOVES 128 // max possible moves from a given position

typedef uint64_t Bitboard;
typedef uint16_t Move;
// move encoding:
// 0-5 bits for from square, 6-11 bits for to square, 12-16 for move flag (promotion, castle, etc...)


typedef struct {
    Bitboard occupancies[3]; // 0 = white, 1 = black, 2 = both
    Bitboard pieces[12]; // one bitboard for each piece

    uint64_t hash; // zobrist hash
} Board;

typedef struct {
    Move moves[MAX_MOVES];
    int size;
} MoveList;





#endif 