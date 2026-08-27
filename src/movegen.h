#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "move.h"
#include "position.h"

int isSquareAttacked(Position* pos, int square, int attackerColour);

int moveIsLegal(Position* pos, Move move);
int moveWasLegal(Position* pos); // check if king is in check

void generateLegalMoves(Position* pos, MoveList* legalMoves);
void generateNoisyMoves(Position* pos, MoveList* noisyMoves);
void generateQuietMoves(Position* pos, MoveList* quietMoves);

#endif