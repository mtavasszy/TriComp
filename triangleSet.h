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
	TriangleSet(int seed, int screenW, int screenH);
	void Initialize(int seed);
	void InitRandom(int seed);
	void InitTriangles();
	float GetMSE(sf::Shader& squaredErrorShader, sf::Shader& getMipMapVal, int maxMipmapLvl, sf::Sprite& targetImageSprite);
	void DrawRenderTexture(sf::RenderTexture& rt);

	void DrawMSETexture(sf::Shader& squaredErrorShader, sf::Sprite& targetImageSprite, sf::RenderTexture& rt);

	void Draw(sf::RenderWindow& window);

	std::mt19937 m_gen;
	
	int m_screenW, m_screenH;
	
	std::uniform_int_distribution<int> m_seedDist;
	std::uniform_real_distribution<float> m_triDist;
	std::uniform_int_distribution<uint32_t> m_colorDist;

	std::vector<sf::ConvexShape> m_triangles;
};

#endif  // TRIANGLESET_H_