#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"

#define MAX_SEARCH_DEPTH 128

extern int timeLimit;

typedef struct {
    int nodes;
    int ply;
    int abort;
    long long startTime;
} NodeState;


int negaMax(Position* pos, int depth, int alpha, int beta, NodeState* state);

void searchPosition(Position* pos, int searchTimeLimit);

#endif