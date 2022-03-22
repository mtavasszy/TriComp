#include "triangleSet.h"

TriangleSet::TriangleSet() {}

TriangleSet::TriangleSet(const TriangleSet* ts)
{
	m_imageW = ts->m_imageW;
	m_imageH = ts->m_imageH;

	for (int i = 0; i < ts->m_triangles.size(); i++) {
		sf::ConvexShape copy_tri = ts->m_triangles[i];
		m_triangles.push_back(copy_tri);
	}

	Mutate();
}

TriangleSet::TriangleSet(int screenW, int screenH)
{
	m_imageW = screenW;
	m_imageH = screenH;
	Initialize();
}

void TriangleSet::Initialize()
{
	InitTriangles();
}

void TriangleSet::InitTriangles()
{
	m_triangles.reserve(TRI_N);

	for (int i = 0; i < TRI_N; i++) {
		AddRandomTriangle();
	}
}

void TriangleSet::AddRandomTriangle()
{
	sf::ConvexShape tri = sf::ConvexShape(3);
	tri.setPoint(0, sf::Vector2f(float(RANDINT(m_imageW)), float(RANDINT(m_imageH))));
	tri.setPoint(1, sf::Vector2f(float(RANDINT(m_imageW)), float(RANDINT(m_imageH))));
	tri.setPoint(2, sf::Vector2f(float(RANDINT(m_imageW)), float(RANDINT(m_imageH))));
	tri.setFillColor(sf::Color(RANDINT(255), RANDINT(255), RANDINT(255), RANDINT(1)));
	m_triangles.push_back(tri);
}

float TriangleSet::GetError(TriSetErrorCompPackage& triSetErrorCompPackage)
{
	return GetAbsoluteError(triSetErrorCompPackage);
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
	tsecp.absErrorShader.setUniform("screenDim", sf::Vector2f(float(m_imageW), float(m_imageH)));

	tsecp.absErrorRenderTexture.draw(tsecp.targetImageSprite, &tsecp.absErrorShader);
}

void TriangleSet::DrawRenderTexture(sf::RenderTexture& rt)
{
	rt.clear(sf::Color::Black);
	for (int i = 0; i < m_triangles.size(); i++) {
		rt.draw(m_triangles[i]);
	}
	//rt.display();
}

float TriangleSet::GetPixelAverageMipMap(TriSetErrorCompPackage& tsecp)
{
	tsecp.absErrorImage = tsecp.absErrorRenderTexture.getTexture().copyToImage();

	float error = 0.f;
	for (int x = 0; x < m_imageW; x++) {
		for (int y = 0; y < m_imageH; y++) {
			sf::Color col = tsecp.absErrorImage.getPixel(x, y);
			error += float(col.r) + float(col.g) + float(col.b);
		}
	}

	error /= float(m_imageW * m_imageH);

	//// get mean absolute average by averaging all pixels in the absError texture
	//tsecp.absErrorRenderTexture.generateMipmap();

	//// copy the lowest level mipmap value into a 1x1 texture 
	//tsecp.getMipmapValShader.setUniform("mipmapTexture", tsecp.absErrorRenderTexture.getTexture());
	//tsecp.getMipmapValShader.setUniform("maxMipmapLvl", tsecp.maxMipmapLvl);
	//tsecp.smolRenderTexture.draw(tsecp.smolSprite, &tsecp.getMipmapValShader);

	//// copy 1x1 texture to cpu and retrieve value
	//sf::Image avgErrorImage;
	//avgErrorImage = tsecp.smolRenderTexture.getTexture().copyToImage();
	//sf::Color resultMipMap = avgErrorImage.getPixel(0, 0);

	//float error = float(resultMipMap.r) + float(resultMipMap.g) + float(resultMipMap.b);

	return error;
}

void TriangleSet::Mutate()
{
	int t = RANDINT(TRI_N);
	int mutType = RANDINT(2);

	if (mutType == 0) {
		MutateVertexValue(t);
	}
	else {
		MutateColorValue(t);
	}
}

void TriangleSet::MutateVertexValue(int t)
{
	int p_i = RANDINT(3);
	sf::Vector2f p = m_triangles[t].getPoint(p_i);

	int axis = RANDINT(2);

	if (axis == 0) {
		m_triangles[t].setPoint(p_i, sf::Vector2f(float(RANDINT(m_imageW)), p.y));
	}
	else {
		m_triangles[t].setPoint(p_i, sf::Vector2f(p.x, float(RANDINT(m_imageH))));
	}

}

void TriangleSet::MutateColorValue(int t)
{

	sf::Color color = m_triangles[t].getFillColor();

	int channel = RANDINT(4);

	if (channel == 0) {
		color.r = RANDINT(256);
	}
	else if (channel == 1) {
		color.g = RANDINT(256);
	}
	else if (channel == 2) {
		color.b = RANDINT(256);
	}
	else if (channel == 3) {
		color.a = RANDINT(256);
	}

	m_triangles[t].setFillColor(color);
}