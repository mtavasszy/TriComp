#include "app.h"

#include <SFML/Graphics.hpp>
#include "config.h"
#include "stopwatch.h"
#include <iomanip>
#include <sstream>
#include <string>

App::App() {}

void App::Run()
{
	Initialize();

	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				m_window.close();
				return;
			}
		}

		Update();
		Draw();
	}
}

void App::Initialize()
{
	LoadImageAndTextures();
	LoadShaders();
	InitWindow();
	InitPlots();

	m_bestTriangleSet = TriangleSet(m_imageW, m_imageH);
}

void App::LoadImageAndTextures()
{
	std::string fname = std::string(TARGET_IMG_PATH) + std::string(TARGET_IMG_FNAME);
	if (!m_targetImage.loadFromFile(fname))
	{
		exit(-1);
	}
	m_imageW = m_targetImage.getSize().x;
	m_imageH = m_targetImage.getSize().y;

	//float imScaleX = float(MAX_IMAGE_DIM) / float(m_imageW);
	//float imScaleY = float(MAX_IMAGE_DIM) / float(m_imageH);

	m_triSetErrorCompPackage.maxMipmapLvl = int(std::log2(std::max(m_imageW, m_imageH)));

	m_targetTexture.loadFromImage(m_targetImage);
	//m_triSetErrorCompPackage.targetImageSprite.scale(imScaleX, imScaleY);
	m_triSetErrorCompPackage.targetImageSprite.setTexture(m_targetTexture, true);

	m_bestRenderTexture.create(m_targetImage.getSize().x, m_targetImage.getSize().y);
	//m_bestImageSprite.scale(imScaleX, imScaleY);
	m_bestImageSprite.setTexture(m_bestRenderTexture.getTexture(), true);
	m_bestImageSprite.setPosition(sf::Vector2f(float(m_targetImage.getSize().x), 0.f));

	m_triSetErrorCompPackage.smolRenderTexture.create(1, 1);
	m_triSetErrorCompPackage.smolSprite = sf::Sprite(m_triSetErrorCompPackage.smolRenderTexture.getTexture());

	m_triSetErrorCompPackage.triangleRenderTexture.create(m_imageW, m_imageH);
	m_triSetErrorCompPackage.absErrorRenderTexture.create(m_imageW, m_imageH);
}

void App::LoadShaders()
{
	if (!m_triSetErrorCompPackage.absErrorShader.loadFromFile("absError.frag", sf::Shader::Fragment))
	{
		exit(-1);
	}
	if (!m_triSetErrorCompPackage.getMipmapValShader.loadFromFile("getMipmapVal.glsl", sf::Shader::Fragment))
	{
		exit(-1);
	}
}

void App::InitWindow()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	m_window.create(sf::VideoMode(m_imageW * 2, m_imageH), "TriComp", sf::Style::Default, settings);

	totalTimeSW = Stopwatch();
}

void App::InitPlots()
{
	m_errorPlot.Create(WINDOW_W, STATISTICS_VIEW_H, STATISTICS_GRAPH_MARGIN_W, STATISTICS_GRAPH_MARGIN_W, STATISTICS_GRAPH_MARGIN_H, STATISTICS_GRAPH_MARGIN_H);
	m_errorPlot.m_sprite.setPosition(sf::Vector2f(0.f, MAX_IMAGE_DIM));

	m_errorPlot.AssignColor(0, sf::Color(180, 255, 180));
	//m_errorPlot.AssignColor(1, sf::Color(255, 255, 255));
	//m_errorPlot.AssignColor(2, sf::Color(255, 180, 180));

}

void App::Update()
{

	TriangleSet mutation(&m_bestTriangleSet);
	float error = mutation.GetError(m_triSetErrorCompPackage);

	if (error < m_lowestError) {
		m_bestTriangleSet = mutation;
		m_lowestError = error;
		m_bestTriangleSet.DrawRenderTexture(m_bestRenderTexture);
		m_n_improvements++;
	}

	m_n_muts++;
	RecordStatistics();

}


void App::RecordStatistics()
{
	m_errorPlot.AddDataPoint(0, m_lowestError);
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_triSetErrorCompPackage.targetImageSprite);
	//m_bestImageSprite.setTexture(m_triSetErrorCompPackage.absErrorRenderTexture.getTexture());

	m_window.draw(m_bestImageSprite);

	//m_errorPlot.Draw();
	//m_window.draw(m_errorPlot.m_sprite);

	m_window.display();

	std::stringstream ss;
	//ss << "TriComp - " << std::fixed << std::setprecision(2) << float(totalTimeSW.elapsed()) / 1000.f << "s";
	ss << "TriComp - " << m_n_improvements << " / " << m_n_muts;

	m_window.setTitle(ss.str());
}
