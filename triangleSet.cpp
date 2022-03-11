#include "triangleSet.h"

TriangleSet::TriangleSet()
{
	std::random_device rd;
	m_gen = std::mt19937(rd());

	m_seedDist = std::uniform_int_distribution<int>(0, INT_MAX);
	m_triDist = std::uniform_real_distribution<float>(0.f, 1.f);
	m_colorDist = std::uniform_int_distribution<uint32_t>(0, UINT32_MAX);
}

void TriangleSet::Initialize(int seed, float screenW, float screenH)
{
	m_gen.seed(seed);

	m_triangles.reserve(N_TRIANGLES);

	for (int i = 0; i < N_TRIANGLES; i++) {
		sf::ConvexShape tri = sf::ConvexShape(3);
		tri.setPoint(0, sf::Vector2f(m_triDist(m_gen) * float(screenW), m_triDist(m_gen) * float(screenH)));
		tri.setPoint(1, sf::Vector2f(m_triDist(m_gen) * float(screenW), m_triDist(m_gen) * float(screenH)));
		tri.setPoint(2, sf::Vector2f(m_triDist(m_gen) * float(screenW), m_triDist(m_gen) * float(screenH)));
		tri.setFillColor(sf::Color(m_colorDist(m_gen)));
		m_triangles.push_back(tri);
	}
}

float TriangleSet::GetMSE(sf::Texture& targetTexture)
{
	// maybe use shared rendertexture?
	return 0.0f;
}

void TriangleSet::DrawRenderTexture(sf::RenderTexture& rt)
{
	for (int i = 0; i < m_triangles.size(); i++) {
		rt.draw(m_triangles[i]);
	}
}

void TriangleSet::Draw(sf::RenderWindow& window)
{
	for (int i = 0; i < m_triangles.size(); i++) {
		window.draw(m_triangles[i]);
	}
}
