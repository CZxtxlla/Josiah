#ifndef INFERENCE_H
#define INFERENCE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../position.h"


#define NNUE_MAGIC_NUMBER 0x4E4E5545
#define MAX_ACTIVE 32

typedef struct {
    int inFeatures;
    int outFeatures;
    int16_t* weight;
    int16_t* bias;
} FeatureTransformer;

typedef struct {
    int inFeatures;
    int outFeatures;
    int8_t* weight;
    int32_t* bias;
} HiddenLayer;

typedef struct {
    int numHidden;
    FeatureTransformer* featureTransformer;
    HiddenLayer** hiddenLayers;
} NNUE;

extern NNUE* model;

// loading
FeatureTransformer* loadFeatureTransformer(FILE* file);
HiddenLayer* loadHiddenLayer(FILE* file);
NNUE* loadNNUE(const char* filepath);

// cleanup
void freeFeatureTransformer(FeatureTransformer* layer);
void freeHiddenLayer(HiddenLayer* layer);
void freeNNUE(NNUE* model);

// accumulator
void initAccumulator(Position* pos, NNUE* model);
void updateAccumulator(Position* pos, NNUE* model, int piece, int sq, int isAdding);
int evaluateNNUEQuantized(const Position* pos, NNUE* model);


#endif