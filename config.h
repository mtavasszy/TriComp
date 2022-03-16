#ifndef CONFIG_H_
#define CONFIG_H_

// APP
constexpr char TARGET_IMG_PATH[] = "images/src/";
constexpr char TARGET_IMG_FNAME[] = "picasso1.jpg";
constexpr int STATISTICS_VIEW_H = 300;
constexpr int STATISTICS_GRAPH_MARGIN_H = 20;
constexpr int STATISTICS_GRAPH_MARGIN_W = 20;

constexpr int SEED = 0;
constexpr bool SET_SEED = true;

// EVOLUTIONARY ALGORITHM
constexpr int GEN_SIZE = 1000;
constexpr float MUTATION_CHANCE = 0.05f;
constexpr float ORDERCHANGE_CHANCE = 0.05f;

// TRIANGLE SET
constexpr int N_TRIANGLES = 20;
constexpr int N_VALBITS = 8;

#endif  // CONFIG_H_