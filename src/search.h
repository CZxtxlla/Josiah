#ifndef SEARCH_H
#define SEARCH_H

#include "move.h"
#include "position.h"

#define MAX_SEARCH_DEPTH 128
#define INFINITY_SCORE 50000

extern int timeLimit;

typedef struct {
    long long nodes;
    int ply;
    int abort;
    long long startTime;

    Move pvTable[MAX_SEARCH_DEPTH][MAX_SEARCH_DEPTH];
    int pvLength[MAX_SEARCH_DEPTH];

    int history[2][64][64]; // stm, from, to
    int killer[2][MAX_SEARCH_DEPTH]; // 0 higher priority (more recent)

} SearchState;


int negaMax(Position* pos, int depth, int alpha, int beta, SearchState* state);

void iterativeDeepening(Position* pos, int maxDepth, int searchTimeLimit);

#endif