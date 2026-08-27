#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"

typedef struct {
    int nodes;
    int ply;

} NodeState;


int negaMax(Position* pos, int depth, int alpha, int beta, NodeState* state);


#endif