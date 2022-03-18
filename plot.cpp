#include "plot.h"

void Plot::Create(float sizeX, float sizeY, float marginLeft, float marginRight, float marginUp, float marginDown)
{
	m_sizeX = sizeX;
	m_sizeY = sizeY;
	m_marginLeft = marginLeft;
	m_marginRight = marginRight;
	m_marginUp = marginUp;
	m_marginDown = marginDown;

	m_renderTexture.create(int(m_sizeX), int(m_sizeY));
	m_sprite.setTexture(m_renderTexture.getTexture(), true);

	m_minVal = FLT_MAX;
	m_maxVal = -FLT_MAX;

	m_bgColor = sf::Color(42, 42, 44);
	m_lineColors;
	m_axisColor = sf::Color(255, 255, 255);
}

void Plot::AssignColor(int series_i, sf::Color col)
{
	if (series_i >= m_lineColors.size()) {
		m_lineColors.push_back(col);
		return;
	}

	m_lineColors[series_i] = col;
}

void Plot::AddDataPoint(int series_i, float val)
{
	m_minVal = std::min(m_minVal, val);
	m_maxVal = std::max(m_maxVal, val);

	if (series_i >= m_data.size()) {
		m_data.push_back(std::vector<float>());
		m_data.back().reserve(512);
		m_data.back().push_back(val);
		return;
	}
	
	m_data[series_i].push_back(val);
}

void Plot::Draw()
{
	m_renderTexture.clear(m_bgColor);

	DrawAxes();
	DrawText();
	DrawGraph();

	m_renderTexture.display();
}

float Plot::ToGraphSpace(float val)
{
	return m_sizeY - m_marginDown - (((val - m_minVal) / (m_maxVal - m_minVal)) * (m_sizeY - m_marginDown - m_marginUp));
}

void Plot::DrawAxes()
{
	float zeroLineY = ToGraphSpace(0);

	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(m_marginLeft, zeroLineY), m_axisColor),
		sf::Vertex(sf::Vector2f(m_sizeX - m_marginRight, zeroLineY), m_axisColor)
	};

	m_renderTexture.draw(line, 2, sf::Lines);
}

void Plot::DrawText()
{
}

void Plot::DrawGraph()
{
	if (!m_data.empty()) {
		float stepSizeX = float(m_sizeX - m_marginLeft - m_marginRight) / float(m_data[0].size());

		for (int s = 0; s < m_data.size(); s++) {
			if (m_data[s].size() > 1) {
				for (int i = 0; i < m_data[s].size() - 1; i++) {
					float x1 = m_marginLeft + i * stepSizeX;
					float x2 = m_marginLeft + (i + 1) * stepSizeX;
					float y1 = ToGraphSpace(m_data[s][i]);
					float y2 = ToGraphSpace(m_data[s][i+1]);

					sf::Vertex line[] =
					{
						sf::Vertex(sf::Vector2f(x1, y1), m_lineColors[s]),
						sf::Vertex(sf::Vector2f(x2, y2), m_lineColors[s])
					};

					m_renderTexture.draw(line, 2, sf::Lines);
				}

			}
		}
	}
}
