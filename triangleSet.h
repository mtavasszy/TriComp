#ifndef TRIANGLESET_H_
#define TRIANGLESET_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "config.h"
#include "Vec2.h"
#include <random>

struct TriSetErrorCompPackage {
	sf::Shader absErrorShader;
	sf::Shader getMipmapValShader;
	int maxMipmapLvl;
	sf::RenderTexture smolRenderTexture;
	sf::Sprite smolSprite;
	sf::RenderTexture triangleRenderTexture;
	sf::RenderTexture absErrorRenderTexture;
	sf::Image absErrorImage;
	sf::Sprite targetImageSprite;
};

class TriangleSet {
public:
	TriangleSet();
	TriangleSet(const TriangleSet* ts);
	TriangleSet(int screenW, int screenH);
	void Initialize();
	void InitTriangles();
	void AddRandomTriangle();

	float GetError(TriSetErrorCompPackage& triSetErrorCompPackage);
	float GetAbsoluteError(TriSetErrorCompPackage& triSetErrorCompPackage);
	void DrawAbsErrorTexture(TriSetErrorCompPackage& triSetErrorCompPackage);
	void DrawRenderTexture(sf::RenderTexture& rt);
	float GetPixelAverageMipMap(TriSetErrorCompPackage& triSetErrorCompPackage);

	void Mutate();
	void MutateVertexValue(int t);
	void MutateColorValue(int t);
	
	int m_imageW, m_imageH;
	
	std::vector<sf::ConvexShape> m_triangles;
};

#endif  // TRIANGLESET_H_