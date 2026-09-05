#include "movepicker.h"
#include "evaluate.h"

int scoreMove(Move m, Position* pos, Move ttMove, SearchState* state) {
    if (m == ttMove) {
        return 100000;
    }
    if (IsPromo(m)) {
        return 30000;
    }
    // range 20000 to 28900
    if (IsCapture(m)) {
        int capturer = pos->squares[MoveFrom(m)] % 6;
        int captured = IsEP(m) ? PAWN : (pos->squares[MoveTo(m)] % 6);

        return (10 * PIECE_TO_SCORE[captured]) - PIECE_TO_SCORE[capturer] + 20000;
    } 

    if (m == state->killer[0][state->ply]) {
        return 19000;
    } else if (m == state->killer[1][state->ply]) {
        return 18000;
    }
    // range 0 to 18000
    return state->history[pos->stm][MoveFrom(m)][MoveTo(m)];
}

void orderMoves(MoveList* movesl, Position* pos, Move ttMove, SearchState* state) {
    int scores[256];

    for (int i = 0; i < movesl->size; i++) {
        scores[i] = scoreMove(movesl->moves[i], pos, ttMove, state);
    }

    for (int i = 0; i < movesl->size - 1; i++) {
        int max_idx = i;

        for (int j = i + 1; j < movesl->size; j++) {
            if (scores[j] > scores[max_idx]) {
                max_idx = j;
            }
        }

        Move tempMove = movesl->moves[i];
        movesl->moves[i] = movesl->moves[max_idx];
        movesl->moves[max_idx] = tempMove;

        int tempScore = scores[i];
        scores[i] = scores[max_idx];
        scores[max_idx] = tempScore;
    }
}

void initPicker(MovePicker* picker, Move ttMove) {
    picker->hashMove = ttMove;
    picker->index = 0;
    picker->phase = HASH_MOVE;
    picker->possibleMoves.size = 0;
}

 Move nextMove(MovePicker* picker, Position* pos, SearchState* state) {
    while (1) {
        switch(picker->phase) {
            case HASH_MOVE:
                picker->phase = GEN_NOISY;
                if (picker->hashMove != 0) {
                    return picker->hashMove;
                }
                break;

            case GEN_NOISY:
                picker->phase = NOISY_MOVES;
                generateNoisyMoves(pos, &picker->possibleMoves);
                for (int i = 0; i < picker->possibleMoves.size; i++) {
                    picker->scores[i] = scoreMove(picker->possibleMoves.moves[i], pos, picker->hashMove, state);
                }
                break;

            case NOISY_MOVES:
                if (picker->index < picker->possibleMoves.size) {
                    int bestIndex = picker->index;
                    for (int j = picker->index + 1; j < picker->possibleMoves.size; j++) {
                        if (picker->scores[j] > picker->scores[bestIndex]) {
                            bestIndex = j;
                        }
                    }

                    // swap bestIndex and picker->index
                    Move tempMove = picker->possibleMoves.moves[picker->index];
                    picker->possibleMoves.moves[picker->index] = picker->possibleMoves.moves[bestIndex];
                    picker->possibleMoves.moves[bestIndex] = tempMove;

                    int tempScore = picker->scores[picker->index];
                    picker->scores[picker->index] = picker->scores[bestIndex];
                    picker->scores[bestIndex] = tempScore;

                    Move m = picker->possibleMoves.moves[picker->index];
                    picker->index++;

                    if (m != picker->hashMove) {
                        return m;
                    }
                } else {
                    picker->phase = GEN_QUIET;
                }
                break;

            case GEN_QUIET:
                picker->phase = QUIET_MOVES;
                picker->index = 0;
                picker->possibleMoves.size = 0;
                generateQuietMoves(pos, &picker->possibleMoves);
                break;

            case QUIET_MOVES:
                if (picker->index < picker->possibleMoves.size) {
                    int bestIndex = picker->index;
                    for (int j = picker->index + 1; j < picker->possibleMoves.size; j++) {
                        if (picker->scores[j] > picker->scores[bestIndex]) {
                            bestIndex = j;
                        }
                    }

                    Move tempMove = picker->possibleMoves.moves[picker->index];
                    picker->possibleMoves.moves[picker->index] = picker->possibleMoves.moves[bestIndex];
                    picker->possibleMoves.moves[bestIndex] = tempMove;

                    int tempScore = picker->scores[picker->index];
                    picker->scores[picker->index] = picker->scores[bestIndex];
                    picker->scores[bestIndex] = tempScore;

                    Move m = picker->possibleMoves.moves[picker->index];
                    picker->index++;

                    if (m != picker->hashMove) {
                        return m;
                    }
                } else {
                    picker->phase = END;
                }
                break;

            case END:
                return NULL_MOVE;

        }
    }
}