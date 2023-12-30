#include "NLNetwork.h"

#include "NLDebug.h"
#include<stdlib.h>
#include<string.h>

void create_rand_network(struct NLNetwork* network, struct NLFormat* format, size_t l_format)
{
	network->max_dim = 0;
	network->num_layers = l_format;
	network->layers = (struct NLLayer*)malloc(l_format * sizeof(struct NLLayer));
	alloc_test(network->layers, "Bad Alloc (CRN1)");
	for (size_t s = 0; s < l_format; ++s) {
		network->max_dim = max(network->max_dim, format[s].w);
		network->layers[s].activ = format[s].act;
		if (_nl_comb_init(&(network->layers[s].combin), format[s].w, format[s].h) == -1)
			alloc_test(NULL, "Bad alloc (CRN2)");
		_nl_comb_rand(&(network->layers[s].combin));
	}
}

void create_copy_network(struct NLNetwork* dest, struct NLNetwork* src)
{
	dest->max_dim = src->max_dim;
	dest->num_layers = src->num_layers;
	dest->layers = (struct NLLayer*)malloc(src->num_layers * sizeof(struct NLLayer));
	alloc_test(dest->layers, "Bad Alloc (CCN)");
	for (size_t s = 0; s < src->num_layers; ++s) {
		dest->layers[s].activ = src->layers[s].activ;
		_nl_comb_copy(&(dest->layers[s].combin), &(src->layers[s].combin));
	}
}

void destroy_network(struct NLNetwork* network)
{
	for (size_t s = 0; s < network->num_layers; ++s) {
		_nl_comb_destroy(&(network->layers[s].combin));
	}
	free(network->layers);
}

// Warning : the function is often used with input = output
void eval_network(struct NLNetwork* network, const float* input, float* process, float* output)
{
	float sum;
	float* entry = input;
	for (size_t s = 0; s < network->num_layers; ++s) { // Sur tous les niveaux
		struct NLLayer* lay = network->layers + s;
		for (size_t j = 0; j < lay->combin.h; ++j) { // A tout ordonnee
			size_t dj = j * lay->combin.w;
			sum = 0;
			for (size_t i = 0; i < lay->combin.w; ++i) // A tout abscisse
				sum += lay->combin.matrix[dj + i] * entry[i];
			process[j] = sum;
		}

		for (size_t j = 0; j < lay->combin.h; ++j)
			output[j] = lay->activ(process[j]);
		
		entry = output;
	}
}

void mix_network(struct NLNetwork* dest, struct NLNetwork* src, float regen_proba, float mut_proba, float mut_factor)
{
	for (size_t s = 0; s < src->num_layers; ++s)
		_nl_comb_mix(&(dest->layers[s].combin), &(src->layers[s].combin), regen_proba, mut_proba, mut_factor);
}


