#include "app.h"

#include <SFML/Graphics.hpp>
#include "config.h"
#include "stopwatch.h"

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
	LoadImageAndTextures();
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

void App::LoadImageAndTextures()
{
	std::string fname = std::string(TARGET_IMG_PATH) + std::string(TARGET_IMG_FNAME);
	if (!m_targetImage.loadFromFile(fname))
	{
		exit(-1);
	}
	m_screenW = m_targetImage.getSize().x;
	m_screenH = m_targetImage.getSize().y;

	m_triSetErrorCompPackage.maxMipmapLvl = int(std::log2(std::max(m_screenW, m_screenH)));

	m_targetImageTexture.loadFromImage(m_targetImage);
	m_triSetErrorCompPackage.targetImageSprite.setTexture(m_targetImageTexture, true);

	m_bestRenderTexture.create(m_targetImage.getSize().x, m_targetImage.getSize().y);
	m_bestImageSprite.setTexture(m_bestRenderTexture.getTexture(), true);
	m_bestImageSprite.setPosition(sf::Vector2f(float(m_targetImage.getSize().x), 0.f));

	m_triSetErrorCompPackage.smolRenderTexture.create(1, 1);
	m_triSetErrorCompPackage.smolSprite = sf::Sprite(m_triSetErrorCompPackage.smolRenderTexture.getTexture());

	m_triSetErrorCompPackage.triangleRenderTexture.create(m_screenW, m_screenH);

	m_triSetErrorCompPackage.absErrorRenderTexture.create(m_screenW, m_screenH);

}

void App::LoadShaders()
{
	if (!m_triSetErrorCompPackage.absErrorShader.loadFromFile("absError.glsl", sf::Shader::Fragment))
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

	m_window.create(sf::VideoMode(m_screenW * 2, m_screenH), "TriComp", sf::Style::Default, settings);
}

void App::InitTriSets()
{
	std::cout << "Generating " << GEN_SIZE << " random triangle sets...\n";
	Stopwatch sw;

	m_lowestError = FLT_MAX;

	m_triangleSets.clear();
	m_triangleSets.reserve(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE; i++) {
		m_triangleSets.push_back(TriangleSet(m_seedDist(m_gen), m_screenW, m_screenH));
	}

	std::cout << "Triset initialization completed! It took " << sw.reset() << "ms\n";
}

void App::Update()
{
	Stopwatch sw;

	RunGeneration();
	SetBest();
	CreateOffspring();

	std::cout << "\n---------------\n";
	std::cout << "Iteration " << ++n_iterations << " finished, total time was " << sw.reset() << " ms\n";
	std::cout << "Lowest error so far is " << m_lowestError << "\n";
	std::cout << "---------------\n\n";
}

bool FitnessSortComp(const std::pair<int, float> f0, const std::pair<int, float> f1)
{
	return (f0.second < f1.second);
}

void App::RunGeneration()
{
	std::cout << "Computing all triangle set errors...\n";
	Stopwatch sw;

	m_fitnessRanking.clear();
	m_fitnessRanking.reserve(GEN_SIZE);

	for (int i = 0; i < m_triangleSets.size(); i++) {
		const float fitness = m_triangleSets[i].GetAbsoluteError(m_triSetErrorCompPackage);
		m_fitnessRanking.push_back(std::pair<int, float>(i, fitness));
	}

	std::cout << "All triangle sets evaluated! It took " << sw.reset() << " ms\n";
	std::cout << "Sorting on lowest error...\n";

	std::sort(m_fitnessRanking.begin(), m_fitnessRanking.end(), FitnessSortComp);

	std::cout << "Sorting completed! It took " << sw.reset() << " ms\n";
}



void App::CreateOffspring()
{
	std::cout << "Creating offspring...\n";
	Stopwatch sw;

	std::vector<TriangleSet> offspring(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE / 2; i++) {
		TriangleSet* t1 = &m_triangleSets[m_fitnessRanking[i].first];
		TriangleSet* t2 = &m_triangleSets[m_fitnessRanking[i + 1].first];

		auto crossBreedPair = t1->CrossBreed(t2);
		offspring[i * 2] = crossBreedPair.first;
		offspring[i * 2 + 1] = crossBreedPair.second;
	}

	m_triangleSets = offspring;

	std::cout << "Offspring generated! It took " << sw.reset() << " ms\n";
}

void App::SetBest()
{
	int bestTriSetId = m_fitnessRanking[0].first;
	std::cout << "Best creature of this generation is " << bestTriSetId << " with an error of " << m_fitnessRanking[0].second << "\n";

	if (m_fitnessRanking[0].second < m_lowestError) {
		m_bestTriangleSet = m_triangleSets[m_fitnessRanking[0].first];
		m_lowestError = m_fitnessRanking[0].second;
	}

	m_bestTriangleSet.DrawRenderTexture(m_bestRenderTexture);
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_triSetErrorCompPackage.targetImageSprite);
	m_window.draw(m_bestImageSprite);

	m_window.display();
}
