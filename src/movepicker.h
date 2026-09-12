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
    KILLER_MOVE_1,
    KILLER_MOVE_2,
    GEN_QUIET,
    QUIET_MOVES,
    END
} PickerPhase;

typedef struct {
    PickerPhase phase;
    Move hashMove;
    Move killer1;
    Move killer2;

    int index;
    MoveList possibleMoves;
    int scores[MAX_MOVES];
} MovePicker;

void initPicker(MovePicker* picker, Move ttMove, SearchState* state);
Move nextMove(MovePicker* picker, Position* pos, SearchState* state);


#endif