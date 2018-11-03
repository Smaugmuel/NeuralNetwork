#include "NeuralNetwork.hpp"
#include <time.h>
#include <stdlib.h>
#include <cmath>

NeuralNetwork::NeuralNetwork()
{
	m_nLayers = 0;
	m_nNeurons = 0;
	m_nWeights = 0;
	m_outputLayerStartIndex = 0;
	m_nNeuronsPerLayer = nullptr;
	m_neuronValues = nullptr;
	m_neuronWeights = nullptr;
}
NeuralNetwork::NeuralNetwork(const NeuralNetwork & original)
{
	m_nLayers = original.m_nLayers;
	m_nNeurons = original.m_nNeurons;
	m_nWeights = original.m_nWeights;
	m_outputLayerStartIndex = original.m_outputLayerStartIndex;
	
	m_nNeuronsPerLayer = new int[m_nLayers];
	m_neuronValues = new float[m_nNeurons];
	m_neuronWeights = new float[m_nWeights];

	for (int i = 0; i < m_nLayers; i++)
	{
		m_nNeuronsPerLayer[i] = original.m_nNeuronsPerLayer[i];
	}
	/*for (int i = 0; i < m_nNeurons; i++)
	{
		m_neuronValues[i] = original.m_neuronValues[i];
	}*/
	for (int i = 0; i < m_nWeights; i++)
	{
		m_neuronWeights[i] = original.m_neuronWeights[i];
	}
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

NeuralNetwork & NeuralNetwork::operator=(const NeuralNetwork & right)
{
	m_nLayers = right.m_nLayers;
	m_nNeurons = right.m_nNeurons;
	m_nWeights = right.m_nWeights;
	m_outputLayerStartIndex = right.m_outputLayerStartIndex;

	delete[] m_nNeuronsPerLayer;
	delete[] m_neuronValues;
	delete[] m_neuronWeights;
	m_nNeuronsPerLayer = new int[m_nLayers];
	m_neuronValues = new float[m_nNeurons];
	m_neuronWeights = new float[m_nWeights];

	for (int i = 0; i < m_nLayers; i++)
	{
		m_nNeuronsPerLayer[i] = right.m_nNeuronsPerLayer[i];
	}
	/*for (int i = 0; i < m_nNeurons; i++)
	{
	m_neuronValues[i] = right.m_neuronValues[i];
	}*/
	for (int i = 0; i < m_nWeights; i++)
	{
		m_neuronWeights[i] = right.m_neuronWeights[i];
	}

	return *this;
}

bool NeuralNetwork::CreateNetwork(int nLayers, const int * nNeuronsPerLayer)
{
	if (nLayers < 2 || !nNeuronsPerLayer)
	{
		return false;
	}

	m_nLayers = nLayers;
	m_nNeuronsPerLayer = new int[nLayers];
	int nNeurons = 0, nWeights = 0;

	// Create values
	for (int i = 0; i < nLayers; i++)
	{
		m_nNeuronsPerLayer[i] = nNeuronsPerLayer[i];
		nNeurons += m_nNeuronsPerLayer[i];
	}
	m_neuronValues = new float[nNeurons];

	// Output layer begins at "end minus last layer"
	m_outputLayerStartIndex = nNeurons - nNeuronsPerLayer[nLayers - 1];

	// Create weights
	for (int i = 0; i < nLayers - 1; i++)
	{
		nWeights += nNeuronsPerLayer[i] * nNeuronsPerLayer[i + 1];
	}
	m_neuronWeights = new float[nWeights];

	// Set random values
	/*for (int i = 0; i < nNeurons; i++)
	{
		m_neuronValues[i] = static_cast<float>(rand()) / RAND_MAX;
	}*/
	for (int i = 0; i < nWeights; i++)
	{
		m_neuronWeights[i] = static_cast<float>(rand()) / RAND_MAX;
	}
	m_nNeurons = nNeurons;
	m_nWeights = nWeights;

	return true;
}
void NeuralNetwork::UpdateValues(float inputs[])
{
	int i_firstNeuronInLayerToUpdate = 0;
	int i_firstNeuronInLayerToUpdateFrom = 0;
	int i_firstWeightBetweenThisAndNextLayer = 0;

	int i_neuronToUpdate, i_neuronToUpdateFrom, i_weight, i_layer;
	int o_neuronToUpdate, o_neuronToUpdateFrom;

	int n_neuronsInLayerToUpdate = 0;
	int n_neuronsInLayerToUpdateFrom = 0;

	// Set input values
	for (int i = 0; i < m_nNeuronsPerLayer[0]; i++)
	{
		m_neuronValues[i] = inputs[i];
	}

	// Pass input values forward, applying weights to next node
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
			
			float sum = 0.0f;

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

				// Update the neuron value, with a weight
				sum +=
					m_neuronValues[i_neuronToUpdateFrom] *
					m_neuronWeights[i_weight];
			}

			// Assign to member
			m_neuronValues[i_neuronToUpdate] =
				std::tanhf(sum);
		}

		// Move "this layer" forward for next iteration
		i_firstNeuronInLayerToUpdateFrom = i_firstNeuronInLayerToUpdate;
		
		// Move weight offset forward for next iteration
		i_firstWeightBetweenThisAndNextLayer +=
			n_neuronsInLayerToUpdateFrom * n_neuronsInLayerToUpdate;
	}
}
int NeuralNetwork::GetBestOutputIndex() const
{
	int i_begin = m_outputLayerStartIndex + 1;
	int i_end = m_outputLayerStartIndex + m_nNeuronsPerLayer[m_nLayers - 1];
	
	// Assume the first output is the best
	int i_best = m_outputLayerStartIndex;
	
	// Update the best neuron
	for (int i = i_begin; i < i_end; i++)
	{
		if (m_neuronValues[i] > m_neuronValues[i_best])
		{
			i_best = i;
		}
	}

	// Return within interval [0, number of output neurons - 1]
	return i_best - m_outputLayerStartIndex;
}

Weights NeuralNetwork::GetWeights()
{
	return { m_neuronWeights, m_nWeights };
}

void NeuralNetwork::RandomizeWeights()
{
	for (int i = 0; i < m_nWeights; i++)
	{
		m_neuronWeights[i] = static_cast<float>(rand()) / RAND_MAX;
	}
}