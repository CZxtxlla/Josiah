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
} SearchState;


int negaMax(Position* pos, int depth, int alpha, int beta, SearchState* state);

void iterativeDeepening(Position* pos, int maxDepth, int searchTimeLimit);

#endif