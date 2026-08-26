#include <stdio.h>
#include "attacks.h"
#include "bits.h"
#include "position.h"
#include "uci.h"
#include "movegen.h"
#include <sys/time.h>

long long getTimeMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL);
}

uint64_t perft(Position* pos, int depth) {
    if (depth == 0) {
        return 1ULL;
    }

    MoveList movesl;
    generateLegalMoves(pos, &movesl);

    if (depth == 1) {
        return (uint64_t) movesl.size;
    }

    Undo undo;
    uint64_t nodes = 0;

    for (int i = 0; i < movesl.size; i++) {
        makeMove(pos, movesl.moves[i], &undo);
        nodes += perft(pos, depth - 1);
        unmakeMove(pos, movesl.moves[i], &undo);
    }

    return nodes;
}

void perftTest(Position* pos, int depth) {
    printf("Running perft test to depth %d...\n", depth);

    long long startTime = getTimeMS();
    long long totalNodes = 0;

    MoveList movesl;
    generateLegalMoves(pos, &movesl);

    Undo undo;

    for (int i = 0; i < movesl.size; i++) {
        makeMove(pos, movesl.moves[i], &undo);

        long long nodes = perft(pos, depth - 1);

        unmakeMove(pos, movesl.moves[i], &undo);
        
        printMove(movesl.moves[i]);
        printf(": %lld\n", nodes);

        totalNodes += nodes;
    }

    long long duration = getTimeMS() - startTime;
    if (duration == 0) {
        duration = 1;
    }

    printf("\nTotal Nodes: %lld\n", totalNodes);
    printf("Time: %lld ms\n", duration);
    printf("NPS: %lld\n", (totalNodes * 1000) / duration);


}


int main() {
    initAttacks();

    Position pos;
    parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", &pos);

    printPosition(&pos);

    perftTest(&pos, 5);
    
    return 0;
}