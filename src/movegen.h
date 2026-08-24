#ifndef MOVEGEN_H
#define MOVEGEN_H


#include "types.h"
#include "move.h"
#include "position.h"

int generateLegalMoves(Position* pos, MoveList moves);
int generateNoisyMoves(Position* pos, MoveList moves);
int generateQuietMoves(Position* pos, MoveList moves);



#endif