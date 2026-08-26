#ifndef EVALUATE_H
#define EVALUATE_H

#include "position.h"

const int PIECE_TO_SCORE[] = {100, 300, 320, 500, 900};

int evaluateLegalPos(Position* pos);


#endif