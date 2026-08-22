#ifndef MOVE_H
#define MOVE_H

#include "types.h"

typedef uint16_t Move;
// move encoding:
// 0-5 bits for from square, 6-11 bits for to square, 12-16 for move flag (promotion, castle, etc...)
/// https://chessprogramming.org/Encoding_Moves#information-required for codes (remaining 4 bits)

typedef struct {
    Move moves[MAX_MOVES];
    int size;
} MoveList;

#define MoveFrom(Move) (move & 63)
#define MoveTo(Move) ((move >> 6) & 63)

#endif