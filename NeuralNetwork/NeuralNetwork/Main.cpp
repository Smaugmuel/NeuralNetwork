#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <chrono>
#include <crtdbg.h>

#include "NeuralNetwork.hpp"
#include "SnakeGame.hpp"
#include "Settings.hpp"
#include "NetworkTrainer.hpp"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<std::chrono::steady_clock> Time;

void ControlSnakeWithKeys(SnakeGame& snake, float dt)
{
	float sensorDistances[N_SENSORS];
	float sensorTypes[N_SENSORS];

	snake.RetrieveSensorValues(sensorDistances, sensorTypes);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		snake.SteerRight(dt);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		snake.SteerLeft(dt);
	}

	snake.Update(dt);
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	sf::RenderWindow window(sf::VideoMode(WNDW, WNDH), "Neural Network");

	// Seed the random function
	srand(static_cast<unsigned int>(time(nullptr)));

	/*
	Neural network settings
	*/
	int nNeuronsPerLayer[] = { N_SENSORS * 2, 3 };
	int nLayers = sizeof(nNeuronsPerLayer) / sizeof(int);
	static const unsigned int N_NETWORKS = 100U;

	/*
	Create games and networks
	*/
	SnakeGame snakeGames[N_NETWORKS];
	NeuralNetwork networks[N_NETWORKS];
	int scores[N_NETWORKS];

	NetworkTrainer trainer;
	

	/*
	Initialize games and networks
	*/
	for (int i = 0; i < N_NETWORKS; i++)
	{
		snakeGames[i].Initialize(N_SENSORS);

		networks[i].CreateNetwork(
			nLayers,
			nNeuronsPerLayer
		);
	}


	Time t1 = Clock::now();
	Time t2 = t1;

	/*bool isEnterDown = false;
	bool wasEnterDown = false;
	bool wasTraining = false;*/
	bool trainingInitiated = false;

	float trainingTime = 10.0f;

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::EventType::Closed)
			{
				window.close();
			}
		}

		// Measure frame time
		t1 = t2;
		t2 = Clock::now();
		long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
		float dt = static_cast<float>(ns * 0.000000001);

		
		/*wasEnterDown = isEnterDown;
		isEnterDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return);*/
		
		// Start a training session
		/*if (isEnterDown && !wasEnterDown)
		{
			trainer.StartTraining(N_NETWORKS, networks, snakeGames, scores, trainingTime);
			wasTraining = true;
		}
		
		trainer.Update(dt);

		// Check if there was a training that just ended
		if (!trainer.IsTraining() && wasTraining)
		{
			trainer.Mutate();
			trainingTime += 2.0f;
			wasTraining = false;
			trainer.StartTraining(N_NETWORKS, networks, snakeGames, scores, trainingTime);
		}*/


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return))
		{
			trainingInitiated = true;
			trainer.StartTraining(N_NETWORKS, networks, snakeGames, scores, trainingTime);
		}

		trainer.Update(dt);

		if (trainingInitiated && !trainer.IsTraining())
		{
			trainer.Mutate();
			trainingTime += 2.0f;
			trainer.StartTraining(N_NETWORKS, networks, snakeGames, scores, trainingTime);
		}


		/*
		Render
		*/
		window.clear();
		for (int i = 0; i < N_NETWORKS; i++)
		{
			window.draw(snakeGames[i]);
		}
		window.display();
	}

	return 0;
}