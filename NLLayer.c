#include "NLLayer.h"

#include "Random.h"
#include "NLDebug.h"
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define RAND_MOD 8388617u



//////////////////////////////////////
// --- Fonctions de combinaison --- //
//////////////////////////////////////

int _compare_uint32_t(const void* e1, const void* e2) {
	if (*(const float*)e1 < *(const float*)e2)
		return -1;
	return *(const float*)e1 != *(const float*)e2;
}

int _nl_comb_init(struct NLFoncComb* combin, size_t w, size_t h)
{
	combin->w = w;
	combin->h = h;
	combin->matrix = (float*)malloc(w * h * sizeof(float));
	if (combin->matrix == NULL)
		return -1;
	memset(combin->matrix, 0, w * h * sizeof(float));
	return 0;
}

int _nl_comb_copy(struct NLFoncComb* dest, struct NLFoncComb* src)
{
	dest->w = src->w;
	dest->h = src->h;
	dest->matrix = (float*)malloc(dest->w * dest->h * sizeof(float));
	if (dest->matrix == NULL)
		return -1;
	memcpy(dest->matrix, src->matrix, src->w * src->h * sizeof(float));
	return 0;
}

void _nl_comb_rand(struct NLFoncComb* combin)
{
	for (int h = 0; h < combin->h; ++h)
		_nl_comb_rand_ranged_sum_layer(combin, h);
}

void _nl_comb_rand_ranged_sum_layer(struct NLFoncComb* combin, size_t h_layer)
{
	float* mat_layer = combin->matrix + (h_layer * combin->w);
	size_t wd = combin->w - 1;

	uint32_t* val = (uint32_t*)malloc(combin->w * sizeof(uint32_t));
	alloc_test(val, "Bad Alloc (RRSL)");
	for (size_t i = 0; i < wd; ++i) {
		val[i] = rd_next() % RAND_MOD;
	}
	val[wd] = RAND_MOD;
	qsort(val, combin->w, sizeof(uint32_t), _compare_uint32_t);

	uint32_t last = 0;
	for (size_t w = 0; w < combin->w; ++w) {
		mat_layer[w] = (float)(val[w] - last) / RAND_MOD;
		last = val[w];
	}
}

void _nl_comb_muta_ranged_layer(struct NLFoncComb* combin, size_t h_layer, float mut_factor)
{
	float* mat_layer = combin->matrix + (h_layer * combin->w);
	uint64_t mut_crit = mut_factor * NL_RAND_MAX;
	float sum = 0;
	for (size_t w = 0; w < combin->w; ++w) {
		if (rd_next() < mut_crit) {
			mat_layer[w] *= (float)(rd_next() % RAND_MOD) / RAND_MOD + .5f;
		}
		sum += mat_layer[w];
	}
	float rect_fact = 1.f / sum;
	for (size_t w = 0; w < combin->w; ++w)
		mat_layer[w] *= rect_fact;
}

void _nl_comb_mix(struct NLFoncComb* dest, struct NLFoncComb* src, float regen_proba, float mut_proba, float mut_factor)
{
#ifdef NL_DEBUG
	if (dest->h != src->h || dest->w != src->w)
		debug_break(BadSizeArgument, "Impossible mixing !");
#endif // NL_DEBUG

	uint64_t mut_crit = mut_proba * NL_RAND_MAX,
		regen_crit = regen_proba * NL_RAND_MAX,
		bread_crit = .5f * NL_RAND_MAX;
	for (size_t h = 0; h < src->h; ++h) {
		if (rd_next() < regen_crit)
			_nl_comb_rand_ranged_sum_layer(dest, h);
		else {
			if (rd_next() < bread_crit)
				memcpy(dest->matrix + h * dest->w, src->matrix + h * src->w, dest->w * sizeof(float));
			if (rd_next() < mut_crit)
				_nl_comb_muta_ranged_layer(dest, h, mut_factor);
		}
	}
}

void _nl_comb_destroy(struct NLFoncComb* combin)
{
	free(combin->matrix);
}



/////////////////////////////////////
// --- Fonctions d'activations --- //
/////////////////////////////////////

float nl_trivalactiv(float x)
{
	float dec = 0.5f - x;
	float cube = dec * dec * dec;
	dec = 1 / (1 + expf(64 * cube));
	if (dec < 0.1f)
		return 0.f;
	if (dec > 0.9f)
		return 1.f;
	if (dec > .45f && dec < .55f)
		return .5f;
	return dec;
}

float nl_bivalactiv(float x)
{
	float dec = 0.5f - x;
	dec = 1 / (1 + expf(8 * dec));
	if (dec < 0.1f)
		return 0.f;
	if (dec > 0.9f)
		return 1.f;
	return dec;
}

float nl_regres(float x)
{
	return 1 / (1 + expf(8 * (.5f - x)));
}