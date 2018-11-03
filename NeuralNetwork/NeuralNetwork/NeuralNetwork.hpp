#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

struct Weights
{
	float* weights;
	int n;
};

class NeuralNetwork
{
public:
	NeuralNetwork();
	NeuralNetwork(const NeuralNetwork& original);
	~NeuralNetwork();

	NeuralNetwork& operator=(const NeuralNetwork& right);

	bool CreateNetwork(int nLayers, const int* nNeuronsPerLayer = nullptr);

	void UpdateValues(float inputs[]);

	int GetBestOutputIndex() const;

	Weights GetWeights();

	void RandomizeWeights();

private:
	int m_nLayers;
	int* m_nNeuronsPerLayer;
	float* m_neuronValues;
	float* m_neuronWeights;

	int m_outputLayerStartIndex;
	int m_nNeurons;
	int m_nWeights;
};

#endif