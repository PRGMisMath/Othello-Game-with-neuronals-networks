#ifndef NEURONAL_NETWORK_HPP
#define NEURONAL_NETWORK_HPP
#include <array>

#define NB_LAYER 5

class NeuronalNetwork {
public:
	NeuronalNetwork();
	void generate(float p_mutate = 1);
	void reproduce(const NeuronalNetwork& conjoint, float p_mutate = 0.1);
	void mutate(float p_mutate = 0.1);
	int getOutput(bool* canPlayOn) const;
	void setInput(float* input);
private:
	void RecalC();
private:
	template <size_t SInput, size_t SOutput>
	struct Neuron {
		float biais[SOutput];
		float synapse[SOutput][SInput];
	};

	float*& m_input;
	float*& m_output;
	float* m_layers[NB_LAYER];
	float p_buffLayers[NB_LAYER-1][64];
	Neuron<64,64> m_genom[NB_LAYER - 1];
};


#endif