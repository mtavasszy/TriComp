#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdlib.h>     /* srand, rand */

#define RANDINT(max) (rand() % max)
#define RANDFLOAT(max) ((rand() / (float)RAND_MAX) * max)
#define ABS(val) ((val) < 0 ? -(val) : (val))
#define CLAMP(val, min, max) ((val) < (min) ? (min) : \
                              (val) > (max) ? (max) : (val))

// APP
constexpr char TARGET_IMG_PATH[] = "images/src/";
constexpr char TARGET_IMG_FNAME[] = "wave.jpg";

constexpr int WINDOW_W = 840;
constexpr int WINDOW_H = 720;
constexpr int MAX_IMAGE_DIM = 420;
constexpr int STATISTICS_VIEW_H = 300;
constexpr int STATISTICS_GRAPH_MARGIN_H = 20;
constexpr int STATISTICS_GRAPH_MARGIN_W = 20;

constexpr int SEED = 0;
constexpr bool SET_SEED = false;

// EVOLUTIONARY ALGORITHM
//constexpr int GEN_SIZE = 1000;
//constexpr float CDF_SLOPE = 1.f;
//constexpr float TRI_MUTATION_CHANCE = 0.1f;
//constexpr float TRI_AMOUNT_PUNISHMENT = 0.001f;

// TRIANGLE SET
constexpr int TRI_N = 1000;
//constexpr int N_VALBITS = 8;

#endif  // CONFIG_H_