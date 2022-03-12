#include "app.h"

#include <SFML/Graphics.hpp>
#include "config.h"

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
	InitRandom();
	LoadImage();
	LoadShaders();
	InitWindow();
	InitTriSets();

	//m_triangleSets[0].DrawRenderTexture(m_bestRenderTexture);
	m_triangleSets[0].DrawMSETexture(m_squaredErrorShader, m_targetImageSprite, m_bestRenderTexture);
	m_triangleSets[0].GetMSE(m_squaredErrorShader, m_getMipmapValShader, m_maxMipmapLvl, m_targetImageSprite);
}

void App::InitRandom()
{
	std::random_device rd;
	m_gen = std::mt19937(rd());

	if (SET_SEED) {
		m_gen.seed(SEED);
		std::cout << "App seed set to " << SEED << "\n";
	}

	m_seedDist = std::uniform_int_distribution<int>(0, INT_MAX);
}

void App::LoadImage()
{
	std::string fname = std::string(TARGET_IMG_PATH) + std::string(TARGET_IMG_FNAME);
	if (!m_targetImage.loadFromFile(fname))
	{
		exit(-1);
	}
	m_screenW = m_targetImage.getSize().x;
	m_screenH = m_targetImage.getSize().y;

	m_maxMipmapLvl = int(std::log2(std::max(m_screenW, m_screenH)));

	m_targetImageTexture.loadFromImage(m_targetImage);
	m_targetImageSprite.setTexture(m_targetImageTexture, true);

	m_bestRenderTexture.create(m_targetImage.getSize().x, m_targetImage.getSize().y);
	m_bestImageSprite.setTexture(m_bestRenderTexture.getTexture(), true);
	m_bestImageSprite.setPosition(sf::Vector2f(float(m_targetImage.getSize().x), 0.f));
}

void App::LoadShaders()
{
	if (!m_squaredErrorShader.loadFromFile("squaredError.glsl", sf::Shader::Fragment)) 
	{
		exit(-1);
	}
	if (!m_getMipmapValShader.loadFromFile("getMipmapVal.glsl", sf::Shader::Fragment))
	{
		exit(-1);
	}
}

void App::InitWindow()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;

	m_window.create(sf::VideoMode(m_screenW * 2, m_screenH), "TriComp", sf::Style::Default, settings);
}

void App::InitTriSets()
{
	m_triangleSets.reserve(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE; i++) {
		m_triangleSets.push_back(TriangleSet(m_seedDist(m_gen), m_screenW, m_screenH));
	}
}

void App::Update()
{
	//RunGeneration();
	//CreateOffspring();
	//SetBest();


	//   eliminate
	//   crossover
	//   mutate
	//	 update best render texture
}

void App::RunGeneration()
{
	m_fitnessRanking.clear();
	m_fitnessRanking.reserve(GEN_SIZE);

	for (int i = 0; i < m_triangleSets.size(); i++) {
		//const float fitness = m_triangleSets[i].GetMSE(m_squaredErrorShader, m_targetImageSprite);
		//m_fitnessRanking.push_back(std::pair<float, int >(fitness, i));
	}
}

bool FitnessSortComp(const std::pair<float, int> f0, const std::pair<float, int> f1)
{
	return (f0.first < f1.first);
}

void App::CreateOffspring()
{
	std::sort(m_fitnessRanking.begin(), m_fitnessRanking.end(), FitnessSortComp);


}

void App::SetBest()
{
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_targetImageSprite);
	m_window.draw(m_bestImageSprite);

	m_window.display();
}
