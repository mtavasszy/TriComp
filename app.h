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
	void Update();
	void Draw();

	std::mt19937 m_gen;
	std::uniform_int_distribution<int> m_seedDist;

	int m_screenW, m_screenH;

	sf::RenderWindow m_window;

	sf::Image m_targetImage;
	sf::Texture m_targetTexture;
	sf::Sprite m_targetSprite;

	sf::RenderTexture m_bestRenderTexture;
	sf::Sprite m_bestSprite;

	std::vector<TriangleSet> m_triangleSets;
};

#endif  // APP_H_