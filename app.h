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
	void LoadImageAndTextures();
	void LoadShaders();
	void InitWindow();
	void InitPlots();
	void Update();
	void RecordStatistics();
	void Draw();

	int m_imageW, m_imageH;

	sf::RenderWindow m_window;
	Stopwatch totalTimeSW;

	TriSetErrorCompPackage m_triSetErrorCompPackage;

	sf::Texture m_targetTexture;
	sf::Image m_targetImage;

	sf::RenderTexture m_bestRenderTexture;
	sf::Sprite m_bestImageSprite;
	TriangleSet m_bestTriangleSet;

	float m_lowestError = FLT_MAX;

	int m_iterations = 0;
	int m_n_muts = 0;
	int m_n_improvements = 0;

	Plot m_errorPlot;
};

#endif  // APP_H_