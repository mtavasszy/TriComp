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
	std::cout << "Generating " << GEN_SIZE << " random triangle sets...\n";

	m_bestMSE = FLT_MAX;

	m_triangleSets.clear();
	m_triangleSets.reserve(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE; i++) {
		m_triangleSets.push_back(TriangleSet(m_seedDist(m_gen), m_screenW, m_screenH));
	}

	std::cout << "Generating completed!\n";
}

void App::Update()
{
	RunGeneration();
	SetBest();
	CreateOffspring();
}

bool FitnessSortComp(const std::pair<int, float> f0, const std::pair<int, float> f1)
{
	return (f0.second < f1.second);
}

void App::RunGeneration()
{
	std::cout << "Computing triangle set MSE...\n";

	m_fitnessRanking.clear();
	m_fitnessRanking.reserve(GEN_SIZE);

	for (int i = 0; i < m_triangleSets.size(); i++) {
		const float fitness = m_triangleSets[i].GetMSE(m_squaredErrorShader, m_getMipmapValShader, m_maxMipmapLvl, m_targetImageSprite);
		m_fitnessRanking.push_back(std::pair<int, float>(i, fitness));
	}

	std::cout << "All triangle sets evaluated!\n";
	std::cout << "Sorting on MSE...\n";

	std::sort(m_fitnessRanking.begin(), m_fitnessRanking.end(), FitnessSortComp);

	std::cout << "Sorting completed!\n";
}



void App::CreateOffspring()
{
	std::cout << "Creating offspring...\n";

	std::vector<TriangleSet> offspring(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE / 4; i++) {
		TriangleSet* t1 = &m_triangleSets[m_fitnessRanking[2*i].first];
		TriangleSet* t2 = &m_triangleSets[m_fitnessRanking[2*i+1].first];

		auto crossBreedPair1 = t1->CrossBreed(t2);
		auto crossBreedPair2 = t1->CrossBreed(t2);
		offspring[i * 4] = crossBreedPair1.first;
		offspring[i * 4 + 1] = crossBreedPair1.second;
		offspring[i * 4 + 2] = crossBreedPair2.second;
		offspring[i * 4 + 3] = crossBreedPair2.second;
	}

	m_triangleSets = offspring;

	std::cout << "Offspring generated!\n";
}

void App::SetBest()
{
	int bestTriSetId = m_fitnessRanking[0].first;
	std::cout << "Best creature so far has a MSE of " << m_bestMSE << "\n";
	std::cout << "Best creature of this generation is " << bestTriSetId << " with a MSE of " << m_fitnessRanking[0].second << "\n";

	if (m_fitnessRanking[0].second < m_bestMSE) {
		m_bestTriangleSet = m_triangleSets[m_fitnessRanking[0].first];
		m_bestMSE = m_fitnessRanking[0].second;
	}

	m_bestTriangleSet.DrawRenderTexture(m_bestRenderTexture);
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_targetImageSprite);
	m_window.draw(m_bestImageSprite);

	m_window.display();
}
