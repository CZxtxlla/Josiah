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
    int32_t score;
    uint8_t flag;
} TTEntry;

extern TTEntry* TTTable;
extern int entries;

void ttInit(int megabytes);
void ttClear();

int ttStore(uint64_t hash, uint8_t depth, Move move, int32_t score, uint8_t flag); // 1 if overwrite existing data point, 0 if not
int ttProbe(uint64_t hash, uint8_t depth, Move* move, int32_t* score, uint8_t* flag); // 1 if successful, 0 if not


#endif