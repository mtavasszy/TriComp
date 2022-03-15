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
	TriangleSet(const TriangleSet* p1, const TriangleSet* p2, int crossOverIndex);
	TriangleSet(int seed, int screenW, int screenH);
	void Initialize(int seed);
	void InitRandom(int seed);
	void InitTriangles();

	float GetAbsoluteError(sf::Shader& absErrorShader, sf::Shader& getMipMapVal, int maxMipmapLvl, sf::Sprite& targetImageSprite, sf::RenderTexture& smolRenderTexture, sf::Sprite smolSprite, sf::RenderTexture& triangleRenderTexture, sf::RenderTexture& absErrorTexture);
	void DrawAbsErrorTexture(sf::Shader& absErrorShader, sf::Sprite& targetImageSprite, sf::RenderTexture& absErrorRenderTexture, sf::RenderTexture& triangleRenderTexture);
	void DrawRenderTexture(sf::RenderTexture& rt);
	float GetPixelAverageMipMap(sf::Shader& getMipMapVal, int maxMipmapLvl, sf::RenderTexture& renderTexture, sf::RenderTexture& smolRenderTexture, sf::Sprite smolSprite);

	std::pair<TriangleSet, TriangleSet> CrossBreed(const TriangleSet* otherParent);
	void Mutate();
	void MutateVertexValue(int t, int p);
	sf::Vector2f MutatePosition(sf::Vector2f vec, bool isX);
	void MutateColorValue(int t, int channel);

	std::mt19937 m_gen;
	
	int m_screenW, m_screenH;
	
	std::uniform_real_distribution<float> m_triValDist;
	std::uniform_int_distribution<uint32_t> m_colorDist;
	std::uniform_int_distribution<int> m_crossOverDist;
	std::uniform_real_distribution<float> m_mutTriDist;
	std::uniform_int_distribution<int> m_mutValDist;
	std::uniform_int_distribution<int> m_mutPosBitDist;
	std::uniform_int_distribution<int> m_mutColBitDist;

	std::vector<sf::ConvexShape> m_triangles;
};

#endif  // TRIANGLESET_H_