#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"

#define MAX_SEARCH_DEPTH 128
#define INFINITY_SCORE 50000

extern int timeLimit;

typedef struct {
    long long nodes;
    int ply;
    int abort;
    long long startTime;
} NodeState;


int negaMax(Position* pos, int depth, int alpha, int beta, NodeState* state);

void iterativeDeepening(Position* pos, int maxDepth, int searchTimeLimit);

#endif