#include "app.h"

#include <SFML/Graphics.hpp>
#include "config.h"

App::App() {
	std::random_device rd;
	m_gen = std::mt19937(rd());

	if (SET_SEED) {
		m_gen.seed(SEED);
		std::cout << "App seed set to " << SEED << "\n";
	}

	m_seedDist = std::uniform_int_distribution<int>(0, INT_MAX);
}

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
	std::string fname = std::string(TARGET_IMG_PATH) + std::string(TARGET_IMG_FNAME);
	if (!m_targetImage.loadFromFile(fname))
	{
		exit(-1);
	}
	m_screenW = m_targetImage.getSize().x;
	m_screenH = m_targetImage.getSize().y;

	m_targetTexture.loadFromImage(m_targetImage);
	m_targetSprite.setTexture(m_targetTexture, true);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	m_window.create(sf::VideoMode(m_screenW, m_screenH), "TriComp", sf::Style::Default, settings);

	m_triangleSets.reserve(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE; i++) {
		TriangleSet tset;
		tset.Initialize(m_seedDist(m_gen), float(m_screenW), float(m_screenH));
		m_triangleSets.push_back(tset);
	}
}

void App::Update()
{
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_renderSprite);

	//for (int i = 0; i < m_triangleSets.size(); i++) {
	//	m_triangleSets[i].Draw(m_window);
	//}

	m_window.display();
}
