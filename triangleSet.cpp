#include "triangleSet.h"

TriangleSet::TriangleSet() {}

TriangleSet::TriangleSet(const TriangleSet* p1, const TriangleSet* p2, int crossOverIndex)
{
	m_gen = p1->m_gen;

	m_screenW = p1->m_screenW;
	m_screenH = p1->m_screenH;

	m_triValDist = p1->m_triValDist;
	m_colorDist = p1->m_colorDist;
	m_crossOverDist = p1->m_crossOverDist;
	m_mutTriDist = p1->m_mutTriDist;
	m_mutValDist = p1->m_mutValDist;
	m_mutPosBitDist = p1->m_mutPosBitDist;
	m_mutColBitDist = p1->m_mutColBitDist;

	m_triangles.clear();
	m_triangles.reserve(N_TRIANGLES);
	for (int i = 0; i < N_TRIANGLES; i++) {
		if (i < crossOverIndex) {
			const sf::ConvexShape tri_copy = p1->m_triangles[i];
			m_triangles.push_back(tri_copy);
		}
		else {
			const sf::ConvexShape tri_copy = p2->m_triangles[i];
			m_triangles.push_back(tri_copy);
		}
	}
}

TriangleSet::TriangleSet(int seed, int screenW, int screenH)
{
	m_screenW = screenW;
	m_screenH = screenH;
	Initialize(seed);
}

void TriangleSet::Initialize(int seed)
{
	InitRandom(seed);
	InitTriangles();
}

void TriangleSet::InitRandom(int seed)
{
	std::random_device rd;
	m_gen = std::mt19937(rd());
	m_gen.seed(seed);

	m_triValDist = std::uniform_real_distribution<float>(0.f, 1.f);
	m_colorDist = std::uniform_int_distribution<uint32_t>(0, UINT32_MAX);
	m_crossOverDist = std::uniform_int_distribution<int>(0, N_TRIANGLES);
	m_mutTriDist = std::uniform_real_distribution<float>(0.f, 1.f);
	m_mutValDist = std::uniform_int_distribution<int>(0, 10); // 10 values in a triangle
	m_mutPosBitDist = std::uniform_int_distribution<int>(0, N_VALBITS - 1);
	m_mutColBitDist = std::uniform_int_distribution<int>(0, 31);
}

void TriangleSet::InitTriangles()
{
	m_triangles.reserve(N_TRIANGLES);

	for (int i = 0; i < N_TRIANGLES; i++) {
		sf::ConvexShape tri = sf::ConvexShape(3);
		tri.setPoint(0, sf::Vector2f(m_triValDist(m_gen) * float(m_screenW), m_triValDist(m_gen) * float(m_screenH)));
		tri.setPoint(1, sf::Vector2f(m_triValDist(m_gen) * float(m_screenW), m_triValDist(m_gen) * float(m_screenH)));
		tri.setPoint(2, sf::Vector2f(m_triValDist(m_gen) * float(m_screenW), m_triValDist(m_gen) * float(m_screenH)));
		tri.setFillColor(sf::Color(m_colorDist(m_gen)));
		m_triangles.push_back(tri);
	}
}

float TriangleSet::GetMSE(sf::Shader& absErrorShader, sf::Shader& getMipMapVal, int maxMipmapLvl, sf::Sprite& targetImageSprite)
{
	// get per-pixel absolute error between target and generated
	sf::RenderTexture absErrorTexture;
	absErrorTexture.create(m_screenW, m_screenH);
	DrawMSETexture(absErrorShader, targetImageSprite, absErrorTexture);

	float pixelAvg = GetPixelAverageMipMap(getMipMapVal, maxMipmapLvl, absErrorTexture);

	return pixelAvg;
}

void TriangleSet::DrawRenderTexture(sf::RenderTexture& rt)
{
	rt.clear(sf::Color::Black);
	for (int i = 0; i < m_triangles.size(); i++) {
		rt.draw(m_triangles[i]);
	}
}

float TriangleSet::GetPixelAverageMipMap(sf::Shader& getMipMapVal, int maxMipmapLvl, sf::RenderTexture& absErrorRenderTexture)
{
	// get mean absolute average by averaging all pixels in the absError texture
	absErrorRenderTexture.generateMipmap();

	sf::Sprite absErrorSprite;
	absErrorSprite.setTexture(absErrorRenderTexture.getTexture());

	// copy the lowest level mipmap value into a 1x1 texture 
	sf::RenderTexture avgMSETexture;
	avgMSETexture.create(1, 1);

	getMipMapVal.setUniform("mipmapTexture", sf::Shader::CurrentTexture);
	getMipMapVal.setUniform("maxMipmapLvl", maxMipmapLvl);

	avgMSETexture.draw(absErrorSprite, &getMipMapVal);

	// copy 1x1 texture to cpu and retrieve value
	sf::Image avgMSEImage = avgMSETexture.getTexture().copyToImage();
	sf::Color resultMipMap = avgMSEImage.getPixel(0, 0);

	return float(resultMipMap.r + resultMipMap.g + resultMipMap.b);
}

void TriangleSet::DrawMSETexture(sf::Shader& absErrorShader, sf::Sprite& targetImageSprite, sf::RenderTexture& rt)
{
	rt.clear(sf::Color::Black);

	sf::RenderTexture triangleImageTexture;
	triangleImageTexture.create(m_screenW, m_screenH);
	DrawRenderTexture(triangleImageTexture);

	absErrorShader.setUniform("targetImgTexture", sf::Shader::CurrentTexture);
	absErrorShader.setUniform("triangleImgTexture", triangleImageTexture.getTexture());

	rt.draw(targetImageSprite, &absErrorShader);
}

std::pair<TriangleSet, TriangleSet> TriangleSet::CrossBreed(const TriangleSet* otherParent)
{
	int crossOverIndex = m_crossOverDist(m_gen);

	TriangleSet c1 = TriangleSet(this, otherParent, crossOverIndex);
	TriangleSet c2 = TriangleSet(otherParent, this, crossOverIndex);

	c1.Mutate();
	c2.Mutate();

	return std::pair<TriangleSet, TriangleSet>(c1, c2);
}

void TriangleSet::Mutate()
{
	//int triId = m_mutTriDist(m_gen);
	for (int t = 0; t < N_TRIANGLES; t++) {
		if (m_mutTriDist(m_gen) < MUTATION_CHANCE) {
			int valId = m_mutValDist(m_gen);

			if (valId < 6) {
				MutateVertexValue(t, valId);
			}
			else {
				MutateColorValue(t, valId - 6);
			}
		}
	}
}

void TriangleSet::MutateVertexValue(int t, int p)
{
	if (p < 3) {
		m_triangles[t].setPoint(p, MutatePosition(m_triangles[t].getPoint(p), true));
	}
	else {
		m_triangles[t].setPoint(p - 3, MutatePosition(m_triangles[t].getPoint(p - 3), false));
	}
}

sf::Vector2f TriangleSet::MutatePosition(sf::Vector2f vec, bool isX)
{
	int bit = m_mutPosBitDist(m_gen);

	if (isX) {
		uint8_t val_int = uint8_t(((vec.x / float(m_screenW)) * 255.f) + 0.5f);
		val_int ^= uint8_t(1) << bit;
		auto newVec = sf::Vector2f((float(val_int) / 255.f) * float(m_screenW), vec.y);
		return sf::Vector2f((float(val_int) / 255.f) * float(m_screenW), vec.y);
	}
	else {
		uint8_t val_int = uint8_t(((vec.y / float(m_screenH)) * 255.f) + 0.5f);
		val_int ^= uint8_t(1) << bit;
		auto newVec = sf::Vector2f(vec.x, (float(val_int) / 255.f) * float(m_screenH));
		return sf::Vector2f(vec.x, (float(val_int) / 255.f) * float(m_screenH));
	}
}

void TriangleSet::MutateColorValue(int t, int channel)
{
	int bit = m_mutColBitDist(m_gen);
	uint32_t color = uint32_t(m_triangles[t].getFillColor().toInteger());

	sf::Color oldColor(color);
	color ^= uint32_t(1) << bit;
	sf::Color newColor(color);

	m_triangles[t].setFillColor(sf::Color(color));
}
