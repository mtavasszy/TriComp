#include "triangleSet.h"

TriangleSet::TriangleSet() {}

TriangleSet::TriangleSet(const TriangleSet* p1, const TriangleSet* p2)
{
	m_gen = p1->m_gen;

	m_imageW = p1->m_imageW;
	m_imageH = p1->m_imageH;

	m_triValDist = p1->m_triValDist;
	m_colorDist = p1->m_colorDist;
	m_crossOverDist = p1->m_crossOverDist;
	m_mutTriDist = p1->m_mutTriDist;
	m_mutValDist = p1->m_mutValDist;
	m_mutPosBitDist = p1->m_mutPosBitDist;
	m_mutColBitDist = p1->m_mutColBitDist;
	m_bigMutTypeDist = p1->m_bigMutTypeDist;

	const auto crossOverDistP1 = std::uniform_int_distribution<int>(1, int(p1->m_triangles.size()));
	const auto crossOverDistP2 = std::uniform_int_distribution<int>(1, int(p2->m_triangles.size()));

	int crossOverP1 = crossOverDistP1(m_gen);
	int crossOverP2 = crossOverDistP2(m_gen);

	m_triangles.clear();
	m_triangles.reserve(crossOverP1 + crossOverP2);

	for (int i = 0; i < crossOverP1; i++) {
		const sf::ConvexShape tri_copy = p1->m_triangles[i];
		m_triangles.push_back(tri_copy);
	}
	for (int i = 0; i < crossOverP2; i++) {
		const sf::ConvexShape tri_copy = p2->m_triangles[p2->m_triangles.size() - crossOverP2 + i];
		m_triangles.push_back(tri_copy);
	}
}

TriangleSet::TriangleSet(int seed, int screenW, int screenH)
{
	m_imageW = screenW;
	m_imageH = screenH;
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
	m_mutTriDist = std::uniform_real_distribution<float>(0.f, 1.f);
	m_mutValDist = std::uniform_int_distribution<int>(0, 10); // 10 values in a triangle
	m_mutPosBitDist = std::uniform_int_distribution<int>(0, N_VALBITS - 1);
	m_mutColBitDist = std::uniform_int_distribution<int>(0, 31);

	m_bigMutTypeDist = std::uniform_int_distribution<int>(0, 2);
}

void TriangleSet::InitTriangles()
{
	m_triangles.reserve(TRI_START_N);

	for (int i = 0; i < TRI_START_N; i++) {
		AddRandomTriangle();
	}
}

float TriangleSet::GetAbsoluteError(TriSetErrorCompPackage& tsecp)
{
	// get per-pixel absolute error between target and generated
	DrawAbsErrorTexture(tsecp);
	return GetPixelAverageMipMap(tsecp);
}

void TriangleSet::DrawAbsErrorTexture(TriSetErrorCompPackage& tsecp)
{
	tsecp.absErrorRenderTexture.clear(sf::Color::Black);
	tsecp.triangleRenderTexture.clear(sf::Color::Black);

	DrawRenderTexture(tsecp.triangleRenderTexture);

	tsecp.absErrorShader.setUniform("targetImgTexture", sf::Shader::CurrentTexture);
	tsecp.absErrorShader.setUniform("triangleImgTexture", tsecp.triangleRenderTexture.getTexture());

	tsecp.absErrorRenderTexture.draw(tsecp.targetImageSprite, &tsecp.absErrorShader);
}

void TriangleSet::DrawRenderTexture(sf::RenderTexture& rt)
{
	rt.clear(sf::Color::Black);
	for (int i = 0; i < m_triangles.size(); i++) {
		rt.draw(m_triangles[i]);
	}
}

float TriangleSet::GetPixelAverageMipMap(TriSetErrorCompPackage& tsecp)
{
	// get mean absolute average by averaging all pixels in the absError texture
	tsecp.absErrorRenderTexture.generateMipmap();

	// copy the lowest level mipmap value into a 1x1 texture 
	tsecp.getMipmapValShader.setUniform("mipmapTexture", tsecp.absErrorRenderTexture.getTexture());
	tsecp.getMipmapValShader.setUniform("maxMipmapLvl", tsecp.maxMipmapLvl);
	tsecp.smolRenderTexture.draw(tsecp.smolSprite, &tsecp.getMipmapValShader);

	// copy 1x1 texture to cpu and retrieve value
	sf::Image avgErrorImage;
	avgErrorImage = tsecp.smolRenderTexture.getTexture().copyToImage();
	sf::Color resultMipMap = avgErrorImage.getPixel(0, 0);

	float MSE = float(resultMipMap.r + resultMipMap.g + resultMipMap.b);

	return MSE + MSE * float(m_triangles.size()) * TRI_AMOUNT_PUNISHMENT;
}

std::pair<TriangleSet, TriangleSet> TriangleSet::CrossBreed(const TriangleSet* otherParent)
{
	TriangleSet c1 = TriangleSet(this, otherParent);
	TriangleSet c2 = TriangleSet(otherParent, this);

	c1.Mutate();
	c2.Mutate();

	return std::pair<TriangleSet, TriangleSet>(c1, c2);
}

void TriangleSet::Mutate()
{
	for (int t = 0; t < m_triangles.size(); t++) {
		// mutate triangle position, color or alpha
		if (m_mutTriDist(m_gen) < TRI_MUTATION_CHANCE) {
			int valId = m_mutValDist(m_gen);

			if (valId < 6) {
				MutateVertexValue(t, valId);
			}
			else {
				MutateColorValue(t, valId - 6);
			}
		}
	}

	// swap position with other triangle
	if (m_mutTriDist(m_gen) < BIG_MUTATION_CHANCE) {
		switch (m_bigMutTypeDist(m_gen))
		{
		case 0:
			SwapRandomTriangle();
			break;
		case 1:
			AddRandomTriangle();
			break;
		case 2:
			RemoveRandomTriangle();
			break;
		default:
			break;
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
		uint8_t val_int = uint8_t(((vec.x / float(m_imageW)) * 255.f) + 0.5f);
		val_int ^= uint8_t(1) << bit;
		auto newVec = sf::Vector2f((float(val_int) / 255.f) * float(m_imageW), vec.y);
		return sf::Vector2f((float(val_int) / 255.f) * float(m_imageW), vec.y);
	}
	else {
		uint8_t val_int = uint8_t(((vec.y / float(m_imageH)) * 255.f) + 0.5f);
		val_int ^= uint8_t(1) << bit;
		auto newVec = sf::Vector2f(vec.x, (float(val_int) / 255.f) * float(m_imageH));
		return sf::Vector2f(vec.x, (float(val_int) / 255.f) * float(m_imageH));
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

void TriangleSet::SwapRandomTriangle()
{
	const auto mutSwapDist = std::uniform_int_distribution<int>(0, int(m_triangles.size()) - 1);
	const int t = mutSwapDist(m_gen);
	std::swap(m_triangles[t], m_triangles[0]);
}

void TriangleSet::AddRandomTriangle()
{
	sf::ConvexShape tri = sf::ConvexShape(3);
	tri.setPoint(0, sf::Vector2f(m_triValDist(m_gen) * float(m_imageW), m_triValDist(m_gen) * float(m_imageH)));
	tri.setPoint(1, sf::Vector2f(m_triValDist(m_gen) * float(m_imageW), m_triValDist(m_gen) * float(m_imageH)));
	tri.setPoint(2, sf::Vector2f(m_triValDist(m_gen) * float(m_imageW), m_triValDist(m_gen) * float(m_imageH)));
	tri.setFillColor(sf::Color(m_colorDist(m_gen)));
	m_triangles.push_back(tri);
}

void TriangleSet::RemoveRandomTriangle()
{
	auto removeDist = std::uniform_int_distribution<int>(0, int(m_triangles.size()) - 1);
	m_triangles.erase(m_triangles.begin() + removeDist(m_gen));
}
