#ifndef CONFIG_H_
#define CONFIG_H_

// APP
constexpr char TARGET_IMG_PATH[] = "images/src/";
constexpr char TARGET_IMG_FNAME[] = "picasso1.jpg";

constexpr int WINDOW_W = 840;
constexpr int WINDOW_H = 720;
constexpr int MAX_IMAGE_DIM = 420;
constexpr int STATISTICS_VIEW_H = 300;
constexpr int STATISTICS_GRAPH_MARGIN_H = 20;
constexpr int STATISTICS_GRAPH_MARGIN_W = 20;

constexpr int SEED = 0;
constexpr bool SET_SEED = true;

// EVOLUTIONARY ALGORITHM
constexpr int GEN_SIZE = 1000;
constexpr float TRI_MUTATION_CHANCE = 0.1f;
constexpr float BIG_MUTATION_CHANCE = 0.2f;
constexpr float TRI_AMOUNT_PUNISHMENT = 1.f;

// TRIANGLE SET
constexpr int TRI_START_N = 40;
constexpr int N_VALBITS = 8;

#endif  // CONFIG_H_