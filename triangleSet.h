#ifndef TRIANGLESET_H_
#define TRIANGLESET_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "config.h"
#include "Vec2.h"
#include <random>

class TriangleSet {
public:
	TriangleSet();
	void Initialize(int seed, float screenW, float screenH);
	float GetMSE(sf::Texture &targetTexture);
	void Draw(sf::RenderWindow& window);

	std::mt19937 m_gen;
	std::uniform_int_distribution<int> m_seedDist;
	std::uniform_real_distribution<float> m_triDist;
	std::uniform_int_distribution<uint32_t> m_colorDist;

	std::vector<sf::ConvexShape> m_triangles;
};

#endif  // TRIANGLESET_H_