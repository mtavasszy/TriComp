#include "triangleSet.h"

TriangleSet::TriangleSet() {}

TriangleSet::TriangleSet(std::vector<const TriangleSet*> parents, const int seed)
{
	InitRandom(seed);

	m_imageW = parents[0]->m_imageW;
	m_imageH = parents[0]->m_imageH;

	// get total amount of triangles from all parents
	int totalTris = 0;
	for (int i = 0; i < parents.size(); i++) {
		totalTris += int(parents[i]->m_triangles.size());
	}

	// get binomial probability of passing a triangle for each parent
	float totalTrisInv = 1.f / float(totalTris);
	std::vector<float> parentP(parents.size());
	for (int i = 0; i < parents.size(); i++) {
		parentP[i] = float(parents[i]->m_triangles.size()) * totalTrisInv;
	}

	std::vector<int> parentCount(parents.size());

	m_triangles.clear();

	int currentParent = -1;
	for (int i = 0; i < totalTris; i++) {
		do {
			currentParent++;
			if (currentParent >= parents.size()) {
				currentParent = 0;
			}
		} while (parentCount[currentParent] >= parents[currentParent]->m_triangles.size());

		if (m_crossDist(m_gen) < parentP[currentParent]) {
			const sf::ConvexShape tri_copy = parents[currentParent]->m_triangles[parentCount[currentParent]];
			m_triangles.push_back(tri_copy);
		}
		parentCount[currentParent]++;
	}

	Mutate();
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
	m_crossDist = std::uniform_real_distribution<float>(0.f, 1.f);
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



float TriangleSet::GetFitness(TriSetErrorCompPackage& triSetErrorCompPackage)
{
	const float maxPossibleError = 1000.f;
	return maxPossibleError - GetAbsoluteError(triSetErrorCompPackage);
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

		if (m_mutTriDist(m_gen) < BIG_MUTATION_CHANCE) {
			switch (m_bigMutTypeDist(m_gen))
			{
			case 0:
				SwapRandomTriangle(t);
				break;

			case 1:
				AddRandomTriangle();
				break;

			case 2: {
				RemoveTriangle(t);
				t--;
			} break;

			default:
				break;
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

void TriangleSet::SwapRandomTriangle(int t)
{
	const auto mutSwapDist = std::uniform_int_distribution<int>(0, int(m_triangles.size()) - 1);
	std::swap(m_triangles[t], m_triangles[mutSwapDist(m_gen)]);
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

void TriangleSet::RemoveTriangle(int t)
{
	m_triangles.erase(m_triangles.begin() + t);
}
