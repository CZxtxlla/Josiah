#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include "move.h"

int scoreMove(Move m, Position* pos, Move ttMove);

void orderMoves(MoveList* movesl, Position* pos, Move ttMove);

#endif