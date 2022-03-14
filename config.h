#ifndef CONFIG_H_
#define CONFIG_H_

// APP
constexpr char TARGET_IMG_PATH[] = "images/src/";
constexpr char TARGET_IMG_FNAME[] = "sunset.jpg";
constexpr int SEED = 0;
constexpr bool SET_SEED = true;

// EVOLUTIONARY ALGORITHM
constexpr int GEN_SIZE = 1000;
constexpr float MUTATION_CHANCE = 0.1f;

// TRIANGLE SET
constexpr int N_TRIANGLES = 50;
constexpr int N_VALBITS = 8;

#endif  // CONFIG_H_