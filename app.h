#ifndef APP_H_
#define APP_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "triangleSet.h"
#include "stopwatch.h"
#include "plot.h"

class App {
public:
	App();
	void Run();

private:
	void Initialize();
	void InitRandom();
	void LoadImageAndTextures();
	void LoadShaders();
	void InitWindow();
	void InitPlots();
	void InitTriSets();
	void Update();
	void RunGeneration();
	void CreateOffspring();
	void SetBest();
	void RecordStatistics();
	void Draw();

	std::mt19937 m_gen;
	std::uniform_int_distribution<int> m_seedDist;

	int m_imageW, m_imageH;

	sf::RenderWindow m_window;
	Stopwatch totalTimeSW;

	TriSetErrorCompPackage m_triSetErrorCompPackage;

	sf::Texture m_targetTexture;
	sf::Image m_targetImage;

	sf::RenderTexture m_bestRenderTexture;
	sf::Sprite m_bestImageSprite;
	TriangleSet m_bestTriangleSet;

	float m_bestFitness;

	int m_iterations = 0;

	std::vector<TriangleSet> m_triangleSets;
	std::vector<std::pair<int, float>> m_fitnessRanking;

	Plot m_errorPlot;
};

#endif  // APP_H_