#include "inference.h"

HiddenLayer* loadHiddenLayer(FILE* file) {
    HiddenLayer* layer = (HiddenLayer*)malloc(sizeof(HiddenLayer));
    if (fread(&layer->inFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }
    if (fread(&layer->outFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }

    int wSize = layer->inFeatures * layer->outFeatures;
    int bSize = layer->outFeatures;

    layer->weight = (int8_t*)malloc(wSize * sizeof(int8_t));
    layer->bias = (int32_t*)malloc(bSize * sizeof(uint32_t));

    fread(layer->weight, sizeof(int8_t), wSize, file);
    fread(layer->bias, sizeof(int32_t), bSize, file);

    return layer;
}

FeatureTransformer* loadFeatureTransformer(FILE* file) {
    FeatureTransformer* layer = (FeatureTransformer*)malloc(sizeof(FeatureTransformer));
    if (fread(&layer->inFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }
    if (fread(&layer->outFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }

    int wSize = layer->inFeatures * layer->outFeatures;
    int bSize = layer->outFeatures;

    layer->weight = (int16_t*)malloc(wSize * sizeof(int16_t));
    layer->bias = (int16_t*)malloc(bSize * sizeof(uint16_t));

    fread(layer->weight, sizeof(int16_t), wSize, file);
    fread(layer->bias, sizeof(int16_t), bSize, file);

    return layer;
}

NNUE* loadNNUE(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Error: Could not open %s\n", filepath);
        return NULL;
    }

    uint32_t magic;
    if (fread(&magic, sizeof(uint32_t), 1, file) != 1 || magic != NNUE_MAGIC_NUMBER) {
        printf("Error: Invalid NNUE file format or magic number.\n");
        return NULL;
    }

    NNUE* model = (NNUE*)malloc(sizeof(NNUE));
    fread(&model->numHidden, sizeof(int), 1, file);

    model->featureTransformer = loadFeatureTransformer(file);

    model->hiddenLayers = (HiddenLayer**)malloc(model->numHidden * sizeof(HiddenLayer*));
    for (int i = 0; i < model->numHidden; i++) {
        model->hiddenLayers[i] = loadHiddenLayer(file);
    }

    fclose(file);

    return model;
}

