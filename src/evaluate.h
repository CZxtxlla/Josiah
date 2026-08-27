#ifndef EVALUATE_H
#define EVALUATE_H

#include "position.h"

#define MATE_SCORE 999999

const int PIECE_TO_SCORE[] = {100, 300, 320, 500, 900};

int evaluateLegalPos(Position* pos);


#endif