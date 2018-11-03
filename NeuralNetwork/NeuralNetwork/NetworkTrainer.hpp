#ifndef NETWORK_TRAINER_HPP
#define NETWORK_TRAINER_HPP

class NeuralNetwork;
class SnakeGame;

enum class MutateStrategy
{
	MUTATE_COPIES_OF_BEST_NETWORKS
};

class NetworkTrainer
{
public:
	NetworkTrainer();
	~NetworkTrainer();

	void StartTraining(int nNetworks, NeuralNetwork* networks, SnakeGame* snakes, int* scores, float time);

	void Update(float dt);

	void ControlSnake(SnakeGame& snake, NeuralNetwork& network, float dt);

	bool IsTraining() const;

	void Mutate();

private:
	void ShellSortNetworksByScore();

	NeuralNetwork * m_networks;
	SnakeGame* m_snakes;
	int* m_scores;
	int m_nNetworks;

	bool m_isTraining;
	float m_timeToTrain;
	float m_timeElapsed;

	float m_maxMutateFactor;
};

#endif