#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include "move.h"
#include "search.h"

int scoreMove(Move m, Position* pos, Move ttMove, SearchState* state);

void orderMoves(MoveList* movesl, Position* pos, Move ttMove, SearchState* state);

#endif