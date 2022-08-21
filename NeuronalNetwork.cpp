#include "NeuronalNetwork.hpp"

#include<random>
#include<ctime>

NeuronalNetwork::NeuronalNetwork() :
	m_genom(),
	m_layers(),
	m_input(m_layers[0]), m_output(m_layers[NB_LAYER - 1]),
	p_buffLayers()
{
	for (int i = 0; i < NB_LAYER-1; ++i)
		m_layers[i+1] = p_buffLayers[i];
}

void NeuronalNetwork::generate(float p_mutate)
{
	memset(m_genom, 0, sizeof(m_genom));
	this->mutate(p_mutate);
}

void NeuronalNetwork::reproduce(const NeuronalNetwork& conjoint, float p_mutate)
{
	std::srand(std::time(0));
	for (int l=0; l<NB_LAYER-1; ++l)
		for (int o = 0; o < 64; ++o)
			if (rand() % 2 == 0) {
				m_genom[l].biais[o] = conjoint.m_genom[l].biais[o];
				memcpy(m_genom[l].synapse[o], conjoint.m_genom[l].synapse[o], 64 * sizeof(float));
			}
	this->mutate(p_mutate);

}

void NeuronalNetwork::mutate(float p_mutate)
{
	const int i_pM = 1 / p_mutate;
	std::srand(std::time(0));
	for (int l = 0; l<NB_LAYER-1; ++l)
		for (int o = 0; o < 64; ++o) {
			if (rand() % i_pM == 0)
				m_genom[l].biais[o] = 10. * rand() / RAND_MAX - 5;
			for (int i = 0; i < 64; ++i)
				if (rand() % i_pM == 0)
					m_genom[l].synapse[o][i] = 2. * rand() / RAND_MAX - 1;
		}
	this->RecalC();
}

int NeuronalNetwork::getOutput(bool* canPlayOn) const
{
	int max_index = 0;
	bool needPlayableSpot = true;

	for (int o = 0; o < 64; ++o)
		if (canPlayOn[o] && (needPlayableSpot || this->m_output[max_index] < this->m_output[o])) {
			max_index = o;
			needPlayableSpot = false;
		}
	return max_index;
}

void NeuronalNetwork::setInput(float* input)
{
	m_layers[0] = input;
}

void NeuronalNetwork::RecalC()
{
	for (int l = 0; l < NB_LAYER - 1; ++l)
		for (int o = 0; o < 64; ++o) {
			m_layers[l + 1][o] = m_genom[l].biais[o];
			for (int i = 0; i < 64; ++i) {
				m_layers[l + 1][o] += m_genom[l].synapse[i][o];
			}
		}
}
