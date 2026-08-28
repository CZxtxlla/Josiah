#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "zobrist.h"

#define LOWERBOUND 0x01
#define UPPERBOUND 0x02
#define EXACT 0x03

// 16 bytes
typedef struct __attribute__((packed)) {
    uint64_t hash;
    uint8_t depth;
    Move move;
    uint32_t score;
    uint8_t flag;
} TTEntry;

extern TTEntry* TTTable;
extern int entries;

void ttInit(int megabytes);
void ttClear();

void ttStore(uint64_t hash, uint8_t depth, Move move, uint32_t score, uint8_t flag);
void ttProbe(uint64_t hash, uint8_t depth, Move* move, uint32_t* score, uint8_t* flag);


#endif