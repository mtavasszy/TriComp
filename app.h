#ifndef APP_H_
#define APP_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "triangleSet.h"

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
	void InitTriSets();
	void Update();
	void RunGeneration();
	void CreateOffspring();
	void SetBest();
	void Draw();

	std::mt19937 m_gen;
	std::uniform_int_distribution<int> m_seedDist;

	int m_screenW, m_screenH;

	sf::RenderWindow m_window;

	TriSetErrorCompPackage m_triSetErrorCompPackage;

	sf::Texture m_targetImageTexture;
	sf::Image m_targetImage;

	sf::RenderTexture m_bestRenderTexture;
	sf::Sprite m_bestImageSprite;
	TriangleSet m_bestTriangleSet;

	float m_lowestError;

	int n_iterations = 0;

	std::vector<TriangleSet> m_triangleSets;
	std::vector<std::pair<int, float>> m_fitnessRanking;
};

#endif  // APP_H_