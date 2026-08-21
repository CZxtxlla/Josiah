#ifndef MOVE_H
#define MOVE_H

#include "types.h"


#define MoveFrom(Move) (move & 63)
#define MoveTo(Move) ((move >> 6) & 63)

#endif