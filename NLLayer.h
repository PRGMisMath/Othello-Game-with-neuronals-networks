#ifndef NL_LAYER_HPP
#define NL_LAYER_HPP


typedef float(*NLFoncActiv)(float);
typedef unsigned long long size_t;

// matrix[i,j] = matrix[i + j*w]
struct NLFoncComb {
	float* matrix;
	size_t w, h;
};


int _cdecl _nl_comb_init(struct NLFoncComb* combin, size_t w, size_t h);
int _cdecl _nl_comb_copy(struct NLFoncComb* dest, struct NLFoncComb* src);
void _cdecl _nl_comb_rand(struct NLFoncComb* combin);
void _cdecl _nl_comb_rand_ranged_sum_layer(struct NLFoncComb* combin, size_t h_layer);
void _cdecl _nl_comb_muta_ranged_layer(struct NLFoncComb* combin, size_t h_layer, float mut_factor);
void _cdecl _nl_comb_mix(struct NLFoncComb* dest, struct NLFoncComb* src, float regen_proba, float mut_proba, float mut_factor);
void _cdecl _nl_comb_destroy(struct NLFoncComb* combin);


struct NLLayer {
	NLFoncActiv activ;
	struct NLFoncComb combin;
};

float _cdecl nl_trivalactiv(float x);
float _cdecl nl_bivalactiv(float x);
float _cdecl nl_regres(float x);


#endif