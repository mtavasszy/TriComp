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
	m_imageW = m_targetImage.getSize().x;
	m_imageH = m_targetImage.getSize().y;

	float imScaleX = float(MAX_IMAGE_DIM) / float(m_imageW);
	float imScaleY = float(MAX_IMAGE_DIM) / float(m_imageH);

	m_triSetErrorCompPackage.maxMipmapLvl = int(std::log2(std::max(m_imageW, m_imageH)));

	m_targetTexture.loadFromImage(m_targetImage);
	m_triSetErrorCompPackage.targetImageSprite.scale(imScaleX, imScaleY);
	m_triSetErrorCompPackage.targetImageSprite.setTexture(m_targetTexture, true);

	m_bestRenderTexture.create(m_targetImage.getSize().x, m_targetImage.getSize().y);
	m_bestImageSprite.scale(imScaleX, imScaleY);
	m_bestImageSprite.setTexture(m_bestRenderTexture.getTexture(), true);
	m_bestImageSprite.setPosition(sf::Vector2f(float(MAX_IMAGE_DIM), 0.f));

	m_triSetErrorCompPackage.smolRenderTexture.create(1, 1);
	m_triSetErrorCompPackage.smolSprite = sf::Sprite(m_triSetErrorCompPackage.smolRenderTexture.getTexture());

	m_triSetErrorCompPackage.triangleRenderTexture.create(m_imageW, m_imageH);
	m_triSetErrorCompPackage.absErrorRenderTexture.create(m_imageW, m_imageH);

	m_statisticsRenderTexture.create(WINDOW_W, STATISTICS_VIEW_H);
	m_statisticsSprite.setTexture(m_statisticsRenderTexture.getTexture());
	m_statisticsSprite.setPosition(sf::Vector2f(0.f, MAX_IMAGE_DIM));
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
	m_window.create(sf::VideoMode(WINDOW_W, WINDOW_H), "TriComp", sf::Style::Default, settings);

	totalTimeSW = Stopwatch();
}

void App::InitTriSets()
{
	std::cout << "Generating " << GEN_SIZE << " random triangle sets...\n";
	Stopwatch sw;

	m_lowestError = FLT_MAX;

	m_triangleSets.clear();
	m_triangleSets.reserve(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE; i++) {
		m_triangleSets.push_back(TriangleSet(m_seedDist(m_gen), m_imageW, m_imageH));
	}

	std::cout << "Triset initialization completed! It took " << sw.reset() << "ms\n";
}

void App::Update()
{
	Stopwatch sw;

	RunGeneration();
	SetBest();
	RecordStatistics();
	CreateOffspring();

	std::cout << "\n---------------\n";
	std::cout << "Iteration " << ++m_iterations << " finished, total time was " << sw.reset() << " ms\n";
	std::cout << "Amount of triangles = " << m_bestTriangleSet.m_triangles.size() << "\n";
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
	std::cout << "Best creature of this generation is " << bestTriSetId << " with an error of " << m_fitnessRanking[0].second << "\n" ;

	if (m_fitnessRanking[0].second < m_lowestError) {
		m_bestTriangleSet = m_triangleSets[m_fitnessRanking[0].first];
		m_lowestError = m_fitnessRanking[0].second;
	}

	m_bestTriangleSet.DrawRenderTexture(m_bestRenderTexture);
}

void App::RecordStatistics()
{
	m_statistics.push_back(StatisticPoint{ m_fitnessRanking[0].second, m_fitnessRanking[GEN_SIZE / 2].second, m_fitnessRanking[GEN_SIZE - 1].second });
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_triSetErrorCompPackage.targetImageSprite);
	m_window.draw(m_bestImageSprite);

	DrawStatistics();
	m_window.draw(m_statisticsSprite);

	m_window.display();

	std::stringstream ss;
	ss << "TriComp - " << std::fixed << std::setprecision(2) << float(totalTimeSW.elapsed()) / 1000.f << "s";

	m_window.setTitle(ss.str());
}

void App::DrawStatistics()
{
	m_statisticsRenderTexture.clear(sf::Color(42, 42, 44));

	// get min/max
	float minError = FLT_MAX;
	float maxError = -FLT_MAX;
	for (int i = 0; i < m_statistics.size(); i++) {
		minError = std::min(minError, m_statistics[i].minError);
		maxError = std::max(maxError, m_statistics[i].minError); // todo make maxerror when plotting all
	}

	// draw axes


	// draw text


	// draw
	if (m_statistics.size() > 1) {
		
		
		float stepSize_x = float(WINDOW_W - 2 * STATISTICS_GRAPH_MARGIN_W) / float(m_statistics.size());
		float stepSize_y = float(STATISTICS_VIEW_H - 2 * STATISTICS_GRAPH_MARGIN_H) / (maxError - minError);

		for (int i = 0; i < m_statistics.size() - 1; i++) {

			float x1 = STATISTICS_GRAPH_MARGIN_W + i * stepSize_x;
			float x2 = STATISTICS_GRAPH_MARGIN_W + (i + 1) * stepSize_x;
			float y1 = STATISTICS_VIEW_H - (STATISTICS_GRAPH_MARGIN_H + (m_statistics[i].minError - minError) * stepSize_y);
			float y2 = STATISTICS_VIEW_H - (STATISTICS_GRAPH_MARGIN_H + (m_statistics[i + 1].minError - minError) * stepSize_y);

			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(x1, y1), sf::Color::White),
				sf::Vertex(sf::Vector2f(x2, y2), sf::Color::White)
			};

			m_statisticsRenderTexture.draw(line, 2, sf::Lines);
		}
	}
	m_statisticsRenderTexture.display();
	m_statisticsSprite.setTexture(m_statisticsRenderTexture.getTexture(), true);
	// draw to window
	//m_window.draw(m_statisticsSprite);
}
