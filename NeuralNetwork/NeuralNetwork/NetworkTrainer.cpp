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
	if (m_nLeftSteers)
	{
		delete[] m_nLeftSteers;
		m_nLeftSteers = nullptr;
	}
	if (m_nRightSteers)
	{
		delete[] m_nRightSteers;
		m_nRightSteers = nullptr;
	}
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

	if (!m_nLeftSteers)
	{
		m_nLeftSteers = new int[nNetworks];
	}
	if (!m_nRightSteers)
	{
		m_nRightSteers = new int[nNetworks];
	}

	for (int i = 0; i < m_nNetworks; i++)
	{
		m_nRightSteers[i] = 0;
		m_nLeftSteers[i] = 0;
		m_snakes[i].ReInitialize();
	}
}

void NetworkTrainer::Update(float dt)
{
	if (m_isTraining)
	{
		m_timeElapsed += dt;
		if (m_timeElapsed >= m_timeToTrain)
		{
			m_isTraining = false;

			// Score networks
			for (int i = 0; i < m_nNetworks; i++)
			{
				m_scores[i] = m_snakes[i].GetSize();
				
				/*float leftPercent = m_nLeftSteers[i] / static_cast<float>(m_nLeftSteers[i] + m_nRightSteers[i]);
				float rightPercent = 1.0f - leftPercent;
				
				// Maximum is 0.25*4, when the percents are equal (both 0.5)
				float a = leftPercent * rightPercent * 4.0f;
				
				// Give more points if the amount of left steers were closer to the amount of right steers
				m_scores[i] *= (1.0f + a);*/

				// Lower score if snake died
				m_scores[i] *= m_snakes[i].IsAlive() ? 1.0f : 0.0f;
			}
			return;
		}

		// Update networks
		for (int i = 0; i < m_nNetworks; i++)
		{
			if (m_snakes[i].IsAlive())
			{
				ControlSnake(m_snakes[i], m_networks[i], i, dt);
			}
		}
	}
}

void NetworkTrainer::ControlSnake(SnakeGame & snake, NeuralNetwork & network, int i, float dt)
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
		m_nLeftSteers[i]++;
		break;
	case 1:
		snake.SteerRight(dt);
		m_nRightSteers[i]++;
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

#ifdef HARD_COPY_BEST
	// Retrieve best weights
	Weights w_best = m_networks[0].GetWeights();

	// Mutate less as score increases
	float maxMutateFactor = 1.0f;
	//float maxMutateFactor = expf(-0.2f * m_scores[0]);

	for (int i_networks = 1; i_networks < m_nNetworks; i_networks++)
	{
		// Retrieve weights to change
		Weights w = m_networks[i_networks].GetWeights();

		for (int i_weight = 0; i_weight < w.n; i_weight++)
		{
			float mutateFactor =
				static_cast<float>(rand()) / RAND_MAX *		// [0, 1]
				maxMutateFactor * 2.0f -					// [0, 2*max]
				maxMutateFactor;							// [-max, max]

			w.weights[i_weight] =
				w_best.weights[i_weight] *
				mutateFactor;
		}
	}
#else
	// Do nothing with the best networks

	// Mutate the next networks from the best networks
	for (int i_networks = nToKeep; i_networks < nToKeep + nToMutate; i_networks++)
	{
		// Mutate less as score increases
		float maxMutateFactor = expf(-0.2f * m_scores[i_networks - nToKeep]);

		// Retrieve best weights
		Weights w_best = m_networks[i_networks - nToKeep].GetWeights();

		// Retrieve weights to change
		Weights w = m_networks[i_networks].GetWeights();
		
		for (int i_weight = 0; i_weight < w.n; i_weight++)
		{
			float mutateFactor =
				static_cast<float>(rand()) / RAND_MAX *		// [0, 1]
				maxMutateFactor * 2.0f -					// [0, 2*max]
				maxMutateFactor;							// [-max, max]

			w.weights[i_weight] =
				w_best.weights[i_weight] *
				mutateFactor;
		}
	}

	// Add new networks by randomizing their weights
	for (int i_network = nToKeep + nToMutate; i_network < m_nNetworks; i_network++)
	{
		m_networks[i_network].RandomizeWeights();
	}
#endif
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