#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "move.h"
#include "position.h"

void generatePseudoMoves(Position* pos, MoveList* pseudoMoves);
void generateLegalMoves(Position* pos, MoveList* legalMoves);
void generateNoisyMoves(Position* pos, MoveList* noisyMoves);
void generateQuietMoves(Position* pos, MoveList* quietMoves);

#endif