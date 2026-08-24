#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "move.h"
#include "position.h"

int moveIsLegal(Position* pos, Move move);
int moveWasLegal(Position* pos); // check if king is in check

int generateLegalMoves(Position* pos, MoveList* moves);
int generateNoisyMoves(Position* pos, MoveList* moves);
int generateQuietMoves(Position* pos, MoveList* moves);



#endif