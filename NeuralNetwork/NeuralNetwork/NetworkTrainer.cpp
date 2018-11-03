#include "NetworkTrainer.hpp"
#include "NeuralNetwork.hpp"
#include "SnakeGame.hpp"
#include <iostream>
#include <string>


NetworkTrainer::NetworkTrainer()
{
}
NetworkTrainer::~NetworkTrainer()
{
}

void NetworkTrainer::StartTraining(int nNetworks, NeuralNetwork * networks, SnakeGame * snakes, int* scores, float time)
{
	m_nNetworks = nNetworks;
	m_networks = networks;
	m_snakes = snakes;
	m_scores = scores;
	m_timeToTrain = time;
	m_timeElapsed = 0.0f;
	m_isTraining = true;
	m_maxMutateFactor = 1.0f;

	for (int i = 0; i < m_nNetworks; i++)
	{
		m_snakes[i].ReInitialize();
	}
}

void NetworkTrainer::Update(float dt)
{
	if (!m_isTraining)
	{
		return;
	}
	else
	{
		m_timeElapsed += dt;
		if (m_timeElapsed >= m_timeToTrain)
		{
			m_isTraining = false;
			return;
		}
	}

	for (int i = 0; i < m_nNetworks; i++)
	{
		if (m_snakes[i].IsAlive())
		{
			ControlSnake(m_snakes[i], m_networks[i], dt);
		}

		// Score based on foods eaten
		// Dying will indirectly affect, since the snake can't eat more if dead
		m_scores[i] = m_snakes[i].GetSize();
	}
}

void NetworkTrainer::ControlSnake(SnakeGame & snake, NeuralNetwork & network, float dt)
{
	float sensorDistances[N_SENSORS];
	float sensorTypes[N_SENSORS];
	float inputs[N_SENSORS * 2];

	snake.RetrieveSensorValues(sensorDistances, sensorTypes);

	for (int i = 0; i < N_SENSORS; i++)
	{
		inputs[i] = sensorDistances[i];
		inputs[N_SENSORS + i] = sensorTypes[i];
	}

	network.UpdateValues(inputs);

	int bestMove = network.GetBestOutputIndex();
	switch (bestMove)
	{
	case 0:
		snake.SteerLeft(dt);
		break;
	case 1:
		snake.SteerRight(dt);
		break;
	case 2:
		break;
	default:
		break;
	}

	snake.Update(dt);
}

bool NetworkTrainer::IsTraining() const
{
	return m_isTraining;
}

void NetworkTrainer::Mutate()
{
	ShellSortNetworksByScore();

	std::cout <<
		"Best\t" << m_scores[0] <<
		"\nMedian\t" << m_scores[m_nNetworks / 2] <<
		"\nWorst\t" << m_scores[m_nNetworks - 1] << "\n\n";


	int nToKeep = m_nNetworks * NETWORKS_TO_KEEP_PERCENT;
	int nToMutate = m_nNetworks * NETWORKS_TO_MUTATE_FROM_BEST_PERCENT;
	int nNew = m_nNetworks - nToMutate - nToKeep;

	// Mutate networks
	for (int i_network = 0; i_network < nToKeep + nToMutate; i_network++)
	{
		// Mutate less as score increases
		float maxMutateFactor = expf(-0.2f * m_scores[i_network]);

		// Retrieve weights to mutate
		Weights w = m_networks[i_network].GetWeights();

		// Mutate weights
		for (int i_weight = 0; i_weight < w.n; i_weight++)
		{
			float mutateFactor =
				static_cast<float>(rand()) / RAND_MAX *		// [0, 1]
				maxMutateFactor * 2.0f -					// [0, 2*max]
				maxMutateFactor;							// [-max, max]

			w.weights[i_weight] *= mutateFactor;
		}
	}

	// Create new networks by randomizing their weights
	for (int i_network = nToKeep + nToMutate; i_network < m_nNetworks; i_network++)
	{
		m_networks[i_network].RandomizeWeights();
	}
}

void NetworkTrainer::ShellSortNetworksByScore()
{
	int n = m_nNetworks;

	for (int gap = n / 2; gap > 0; gap /= 2)
	{
		for (int i = gap; i < n; i++)
		{
			NeuralNetwork tempNetwork = m_networks[i];
			int tempScore = m_scores[i];
			int j;

			for (j = i; j >= gap && m_scores[j - gap] < tempScore; j -= gap)
			{
				m_networks[j] = m_networks[j - gap];
				m_scores[j] = m_scores[j - gap];
			}
			m_networks[j] = tempNetwork;
			m_scores[j] = tempScore;
		}
	}
}