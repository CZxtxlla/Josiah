#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include "movegen.h"
#include "search.h"

int scoreMove(Move m, Position* pos, Move ttMove, SearchState* state);

void orderMoves(MoveList* movesl, Position* pos, Move ttMove, SearchState* state);

typedef enum {
    HASH_MOVE,
    GEN_NOISY,
    NOISY_MOVES,
    GEN_QUIET,
    QUIET_MOVES,
    END
} PickerPhase;

typedef struct {
    PickerPhase phase;
    Move hashMove;

    int index;
    MoveList possibleMoves;
    int scores[MAX_MOVES];
} MovePicker;

void initPicker(MovePicker* picker, Move ttMove);
Move nextMove(MovePicker* picker, Position* pos, SearchState* state);


#endif