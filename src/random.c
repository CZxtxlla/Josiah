#include "random.h"


uint64_t startState = 358279427;

uint64_t random_U64() {
    uint64_t state = startState;
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    startState = state;
    return state;
}

uint64_t randomMagic() {
    return random_U64() & random_U64() & random_U64();
}