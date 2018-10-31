#include "NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork()
{
	m_nLayers = 0;
	m_nNeuronsPerLayer = nullptr;
	m_neuronValues = nullptr;
	m_neuronWeights = nullptr;
}
NeuralNetwork::~NeuralNetwork()
{
	if (m_neuronWeights)
	{
		delete[] m_neuronWeights;
		m_neuronWeights = nullptr;
	}
	if (m_neuronValues)
	{
		delete[] m_neuronValues;
		m_neuronValues = nullptr;
	}
	if (m_nNeuronsPerLayer)
	{
		delete[] m_nNeuronsPerLayer;
		m_nNeuronsPerLayer = nullptr;
	}
}

bool NeuralNetwork::CreateNetwork(int nLayers, const int * nNeuronsPerLayer)
{
	if (nLayers < 2 || !nNeuronsPerLayer)
	{
		return false;
	}

	m_nLayers = nLayers;
	m_nNeuronsPerLayer = new int[nLayers];

	int nNeurons = 0;
	for (int i = 0; i < nLayers; i++)
	{
		m_nNeuronsPerLayer[i] = nNeuronsPerLayer[i];
		nNeurons += m_nNeuronsPerLayer[i];
	}
	m_neuronValues = new float[nNeurons];

	int nWeights = 0;
	for (int i = 0; i < nLayers - 1; i++)
	{
		nWeights += nNeuronsPerLayer[i] * nNeuronsPerLayer[i + 1];
	}
	m_neuronWeights = new float[nWeights];

	return true;
}
void NeuralNetwork::UpdateValues()
{
	// First layer begins at 0
	int i_firstNeuronInLayerToUpdate = 0;
	int i_firstNeuronInLayerToUpdateFrom = 0;

	// First weight begins at 0
	int i_firstWeightBetweenThisAndNextLayer = 0;

	// Loop indices and offsets
	int i_neuronToUpdate, i_neuronToUpdateFrom, i_weight, i_layer;
	int o_neuronToUpdate, o_neuronToUpdateFrom;

	// Numbuer of neurons in current and next layer
	int n_neuronsInLayerToUpdate = 0;
	int n_neuronsInLayerToUpdateFrom = 0;

	for (i_layer = 0; i_layer < m_nLayers - 1; i_layer++)
	{
		n_neuronsInLayerToUpdateFrom = m_nNeuronsPerLayer[i_layer];
		n_neuronsInLayerToUpdate = m_nNeuronsPerLayer[i_layer + 1];

		// Next layer begins after this one
		i_firstNeuronInLayerToUpdate +=
			n_neuronsInLayerToUpdateFrom;

		for (o_neuronToUpdate = 0; o_neuronToUpdate < n_neuronsInLayerToUpdate; o_neuronToUpdate++)
		{
			// Index of which neuron to update
			i_neuronToUpdate =
				i_firstNeuronInLayerToUpdate +
				o_neuronToUpdate;
			
			for (o_neuronToUpdateFrom = 0; o_neuronToUpdateFrom < n_neuronsInLayerToUpdateFrom; o_neuronToUpdateFrom++)
			{
				// Index of which neuron to update from
				i_neuronToUpdateFrom =
					i_firstNeuronInLayerToUpdateFrom +
					o_neuronToUpdateFrom;
				
				// Index of the weight between the two neurons
				i_weight =
					i_firstWeightBetweenThisAndNextLayer +
					n_neuronsInLayerToUpdateFrom * o_neuronToUpdate +
					o_neuronToUpdateFrom;

				// Update the neuron, with a weight
				m_neuronValues[i_neuronToUpdate] +=
					m_neuronValues[i_neuronToUpdateFrom] *
					m_neuronWeights[i_weight];
			}
		}

		// Move "this layer" forward for next iteration
		i_firstNeuronInLayerToUpdateFrom = i_firstNeuronInLayerToUpdate;
		
		// Move weight offset forward for next iteration
		i_firstWeightBetweenThisAndNextLayer +=
			n_neuronsInLayerToUpdateFrom * n_neuronsInLayerToUpdate;
	}
}