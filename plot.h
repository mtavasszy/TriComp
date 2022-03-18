#ifndef PLOT_H_
#define PLOT_H_

#include <SFML/Graphics.hpp>
#include <vector>

class Plot {
public:
	Plot() {}
	void Create(float sizeX, float sizeY, float marginLeft, float marginRight, float marginUp, float marginDown);
	
	void AssignColor(int series_i, sf::Color col);
	void AddDataPoint(int series_i, float val);
	void Draw();

	float m_sizeX, m_sizeY, m_marginLeft, m_marginRight, m_marginUp, m_marginDown;
	sf::Color m_bgColor;
	std::vector<sf::Color> m_lineColors;
	sf::Color m_axisColor;
	
	sf::Sprite m_sprite;
	sf::RenderTexture m_renderTexture;

private:
	float ToGraphSpace(float val);
	void DrawAxes();
	void DrawText();
	void DrawGraph();

	float m_minVal, m_maxVal;
	std::vector<std::vector<float>> m_data;
};

#endif  // PLOT_H_
