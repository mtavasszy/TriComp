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
	InitConstants();
	LoadImageAndTextures();
	LoadShaders();
	InitWindow();
	InitPlots();
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

void App::InitConstants()
{
	// CDF for weighted random
	m_rankingCDF.reserve(GEN_SIZE);
	m_rankingCDF.push_back(float(GEN_SIZE));

	for (int i = 1; i < GEN_SIZE; i++) {
		m_rankingCDF.push_back(float(GEN_SIZE - i) + m_rankingCDF[i - 1]);
	}

	m_cdfDist = std::uniform_real_distribution<float>(0, m_rankingCDF.back());
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

void App::InitPlots()
{
	m_errorPlot.Create(WINDOW_W, STATISTICS_VIEW_H, STATISTICS_GRAPH_MARGIN_W, STATISTICS_GRAPH_MARGIN_W, STATISTICS_GRAPH_MARGIN_H, STATISTICS_GRAPH_MARGIN_H);
	m_errorPlot.m_sprite.setPosition(sf::Vector2f(0.f, MAX_IMAGE_DIM));

	m_errorPlot.AssignColor(0, sf::Color(180, 255, 180));
	m_errorPlot.AssignColor(1, sf::Color(255, 255, 255));
	m_errorPlot.AssignColor(2, sf::Color(255, 180, 180));

}

void App::InitTriSets()
{
	std::cout << "Generating " << GEN_SIZE << " random triangle sets...\n";
	Stopwatch sw;

	m_bestFitness = -FLT_MAX;

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
	std::cout << "Highest fitness so far is " << m_bestFitness << "\n";
	std::cout << "---------------\n\n";
}

bool FitnessSortComp(const std::pair<int, float> f0, const std::pair<int, float> f1)
{
	return (f0.second > f1.second);
}

void App::RunGeneration()
{
	std::cout << "Computing all triangle set fitnesses...\n";
	Stopwatch sw;

	m_fitnessRanking.clear();
	m_fitnessRanking.reserve(GEN_SIZE);

	for (int i = 0; i < m_triangleSets.size(); i++) {
		const float fitness = m_triangleSets[i].GetFitness(m_triSetErrorCompPackage);
		m_fitnessRanking.push_back(std::pair<int, float>(i, fitness));
	}

	std::cout << "All triangle sets evaluated! It took " << sw.reset() << " ms\n";
	std::cout << "Sorting on highest fitness...\n";

	std::sort(m_fitnessRanking.begin(), m_fitnessRanking.end(), FitnessSortComp);

	std::cout << "Sorting completed! It took " << sw.reset() << " ms\n";
}

void App::CreateOffspring()
{
	std::cout << "Creating offspring...\n";
	Stopwatch sw;

	std::vector<TriangleSet> offspring;
	offspring.reserve(GEN_SIZE);

	for (int i = 0; i < GEN_SIZE; i++) {
		int i1 = GetCDFDraw();
		int i2;
		do {
			i2 = GetCDFDraw();
		} while (i1 == i2);

		TriangleSet* t1 = &m_triangleSets[m_fitnessRanking[i1].first];
		TriangleSet* t2 = &m_triangleSets[m_fitnessRanking[i2].first];

		TriangleSet child = TriangleSet(std::vector<const TriangleSet*>{t1, t2}, m_seedDist(m_gen));

		offspring.push_back(child);
	}

	m_triangleSets = offspring;

	std::cout << "Offspring generated! It took " << sw.reset() << " ms\n";
}

int App::GetCDFDraw()
{
	const float r = m_cdfDist(m_gen);

	for (int i = 0; i < m_rankingCDF.size(); i++) {
		if (m_rankingCDF[i] > r)
			return i;
	}
	return int(m_rankingCDF.size()) - 1;
}

void App::SetBest()
{
	int bestTriSetId = m_fitnessRanking[0].first;
	std::cout << "Best creature of this generation is " << bestTriSetId << " with a fitness of " << m_fitnessRanking[0].second << "\n" ;

	if (m_fitnessRanking[0].second > m_bestFitness) {
		m_bestTriangleSet = m_triangleSets[m_fitnessRanking[0].first];
		m_bestFitness = m_fitnessRanking[0].second;
	}

	m_bestTriangleSet.DrawRenderTexture(m_bestRenderTexture);
}

void App::RecordStatistics()
{
	m_errorPlot.AddDataPoint(0, m_fitnessRanking[0].second);
	m_errorPlot.AddDataPoint(1, m_fitnessRanking[GEN_SIZE / 2].second);
	m_errorPlot.AddDataPoint(2, m_fitnessRanking[GEN_SIZE - 1].second);
}

void App::Draw()
{
	m_window.clear();

	m_window.draw(m_triSetErrorCompPackage.targetImageSprite);
	m_window.draw(m_bestImageSprite);

	m_errorPlot.Draw();
	m_window.draw(m_errorPlot.m_sprite);

	m_window.display();

	std::stringstream ss;
	ss << "TriComp - " << std::fixed << std::setprecision(2) << float(totalTimeSW.elapsed()) / 1000.f << "s";

	m_window.setTitle(ss.str());
}
