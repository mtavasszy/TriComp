#include "triangleSet.h"

TriangleSet::TriangleSet(){}

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

	m_seedDist = std::uniform_int_distribution<int>(0, INT_MAX);
	m_triDist = std::uniform_real_distribution<float>(0.f, 1.f);
	m_colorDist = std::uniform_int_distribution<uint32_t>(0, UINT32_MAX);
}

void TriangleSet::InitTriangles()
{
	m_triangles.reserve(N_TRIANGLES);

	for (int i = 0; i < N_TRIANGLES; i++) {
		sf::ConvexShape tri = sf::ConvexShape(3);
		tri.setPoint(0, sf::Vector2f(m_triDist(m_gen) * float(m_screenW), m_triDist(m_gen) * float(m_screenH)));
		tri.setPoint(1, sf::Vector2f(m_triDist(m_gen) * float(m_screenW), m_triDist(m_gen) * float(m_screenH)));
		tri.setPoint(2, sf::Vector2f(m_triDist(m_gen) * float(m_screenW), m_triDist(m_gen) * float(m_screenH)));
		tri.setFillColor(sf::Color(m_colorDist(m_gen)));
		m_triangles.push_back(tri);
	}
}

float TriangleSet::GetMSE(sf::Shader &squaredErrorShader, sf::Shader &getMipMapVal, int maxMipmapLvl, sf::Sprite& targetImageSprite)
{
	// get per-pixel squared error between target and generated
	sf::RenderTexture squaredErrorTexture;
	squaredErrorTexture.create(m_screenW, m_screenH);
	DrawMSETexture(squaredErrorShader, targetImageSprite, squaredErrorTexture);

	float pixelAvg = GetPixelAverageMipMap(getMipMapVal, maxMipmapLvl, squaredErrorTexture);
	//float pixelAvg = GetPixelAverageCPU(squaredErrorTexture);

	return pixelAvg;
}

void TriangleSet::DrawRenderTexture(sf::RenderTexture& rt)
{
	for (int i = 0; i < m_triangles.size(); i++) {
		rt.draw(m_triangles[i]);
	}
}

float TriangleSet::GetPixelAverageMipMap(sf::Shader& getMipMapVal, int maxMipmapLvl, sf::RenderTexture& renderTexture)
{
	// get mean squared average by averaging all pixels in the squaredError texture
	renderTexture.generateMipmap();

	sf::Sprite squaredErrorSprite;
	squaredErrorSprite.setTexture(renderTexture.getTexture());

	// copy the lowest level mipmap value into a 1x1 texture 
	sf::RenderTexture avgMSETexture;
	avgMSETexture.create(1, 1);

	getMipMapVal.setUniform("mipmapTexture", sf::Shader::CurrentTexture);
	getMipMapVal.setUniform("maxMipmapLvl", maxMipmapLvl);

	avgMSETexture.draw(squaredErrorSprite, &getMipMapVal);

	// copy 1x1 texture to cpu and retrieve value
	sf::Image avgMSEImage = avgMSETexture.getTexture().copyToImage();
	sf::Color resultMipMap = avgMSEImage.getPixel(0, 0);

	return float(resultMipMap.r + resultMipMap.g + resultMipMap.b);
}

float TriangleSet::GetPixelAverageCPU(sf::RenderTexture& renderTexture)
{
	sf::Image image = renderTexture.getTexture().copyToImage();

	float r = 0.f;
	float g = 0.f;
	float b = 0.f;

	for (int x = 0; x < int(image.getSize().x); x++) {
		for (int y = 0; y < int(image.getSize().y); y++) {
			sf::Color c = image.getPixel(x,y);
			r += float(c.r);
			g += float(c.g);
			b += float(c.b);
		}
	}

	float nPixels = float(image.getSize().x * image.getSize().y);

	return (r + g + b) / nPixels;
}

void TriangleSet::DrawMSETexture(sf::Shader& squaredErrorShader, sf::Sprite& targetImageSprite, sf::RenderTexture& rt)
{
	sf::RenderTexture triangleImageTexture;
	triangleImageTexture.create(m_screenW, m_screenH);
	DrawRenderTexture(triangleImageTexture);

	squaredErrorShader.setUniform("targetImgTexture", sf::Shader::CurrentTexture);
	squaredErrorShader.setUniform("triangleImgTexture", triangleImageTexture.getTexture());

	rt.draw(targetImageSprite, &squaredErrorShader);

}

void TriangleSet::Draw(sf::RenderWindow& window)
{
	for (int i = 0; i < m_triangles.size(); i++) {
		window.draw(m_triangles[i]);
	}
}
