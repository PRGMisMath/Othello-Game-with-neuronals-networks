#ifndef NL_NETWORK_HPP
#define NL_NETWORK_HPP

#include "NLLayer.h"


struct NLFormat {
	size_t w, h;
	NLFoncActiv act;
};

struct NLNetwork {
	struct NLLayer* layers;
	size_t num_layers;
	size_t max_dim;
};

void _cdecl create_rand_network(struct NLNetwork* network, struct NLFormat* format, size_t l_format);
void _cdecl create_copy_network(struct NLNetwork* dest, struct NLNetwork* src);
void _cdecl destroy_network(struct NLNetwork* network);
void _cdecl eval_network(struct NLNetwork* network, const float* input, float* process, float* output);
void _cdecl mix_network(struct NLNetwork* dest, struct NLNetwork* src, float regen_proba, float mut_proba, float mut_factor);

struct TrainAI {
	struct NLNetwork network;
	int comp_score;
	int ref_score;
};


#endif // !NL_NETWORK_HPP
