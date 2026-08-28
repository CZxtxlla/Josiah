#include "transposition.h"

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