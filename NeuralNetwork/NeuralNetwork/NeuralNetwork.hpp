#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

class NeuralNetwork
{
public:
	NeuralNetwork();
	virtual ~NeuralNetwork();

	bool CreateNetwork(int nLayers, const int* nNeuronsPerLayer = nullptr);

	void UpdateValues();

private:
	int m_nLayers;
	int* m_nNeuronsPerLayer;
	float* m_neuronValues;
	float* m_neuronWeights;
};

#endif