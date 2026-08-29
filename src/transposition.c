#include "transposition.h"
#include <string.h>
#include <stdlib.h>

TTEntry* TTTable;
int entries;

void ttInit(int megabytes) {
    long long hash_size = megabytes * 1024 * 1024; // in bytes

    entries = hash_size / sizeof(TTEntry);

    if (TTTable != NULL) {
        free(TTTable);
    }

    TTTable = (TTEntry*)malloc(sizeof(TTEntry) * entries);
    if (TTTable == NULL) {
        fprintf(stderr, "Error: problem initializing TT.\n");
        exit(1);
    } else {
        printf("TT initialized with %d MB and %d entries\n", megabytes, entries);
        ttClear();
    }

}

void ttClear() {
    if (!TTTable) {
        return;
    }
    memset(TTTable, 0, sizeof(TTEntry) * entries);
}

int ttStore(uint64_t hash, uint8_t depth, Move move, uint32_t score, uint8_t flag) {
    int index = hash % entries;
    int overwrite = 0;
    if (TTTable[index].hash != hash) {
        overwrite = 1;
    }
    TTTable[index].depth = depth;
    TTTable[index].hash = hash;
    TTTable[index].score = score;
    TTTable[index].flag = flag;
    TTTable[index].move = move;

    return overwrite;
}

int ttProbe(uint64_t hash, uint8_t depth, Move* move, uint32_t* score, uint8_t* flag) {
    int index = hash % entries;
    TTEntry entry = TTTable[index];
    if (entry.hash != hash) {
        return 0;
    }

    if (entry.depth >= depth) {
        // only usable if depth is greater than or equal
        *score = entry.score;
        *flag = entry.flag;
        *move = entry.move;
        return 1;
    }

    return 0;
}