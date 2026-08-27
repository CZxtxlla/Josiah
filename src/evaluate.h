#ifndef EVALUATE_H
#define EVALUATE_H

#include "position.h"

#define MATE_SCORE 999999

extern const int PIECE_TO_SCORE[5];

int evaluateLegalPos(Position* pos);


#endif