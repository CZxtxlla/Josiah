#include "uci.h"
#include "movegen.h"

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

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

void uciLoop() {
    char line[4096];

    Position pos;
    parseFen(STARTPOS, &pos);

    setbuf(stdout, NULL);

    while(fgets(line, sizeof(line), stdin)) {
        if (!strncmp(line, "uci", 3)) {
            printf("Josiah chess engine\n");

        } else if (!strncmp(line, "perft", 5)) {
            strtok(line, " ");
            char* d = strtok(NULL, " ") ? : "5";
            char* fen = strtok(NULL, "\0") ? : STARTPOS;

            int depth = atoi(d);
            parseFen(fen, &pos);
            perftTest(&pos, depth);
        }
    }
}