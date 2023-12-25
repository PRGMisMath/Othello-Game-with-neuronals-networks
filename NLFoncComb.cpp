#include "NLFoncComb.hpp"

#include<memory>
#include "NLDebug.hpp"
#include "Random.hpp"
#include<vector>
#include<algorithm>

#define RAND_MOD 8388617u

nl::FoncComb::FoncComb() :
	m_w(0), m_h(0), m_matrix(NULL)
{
}

nl::FoncComb::FoncComb(size_t w, size_t h) 
{
	init(w, h);
}

nl::FoncComb::~FoncComb()
{
	free(m_matrix);
}

void nl::FoncComb::init(size_t w, size_t h)
{
	m_w = w; m_h = h;
	m_matrix = (float*)malloc(w * h * sizeof(float));
	if (m_matrix == NULL)
		throw std::bad_alloc();
	memset(m_matrix, 0, w * h * sizeof(float));
}

void nl::FoncComb::rand_gen()
{
	for (int h = 0; h < m_h; ++h)
		RandSumRangedLayer(h);
}

void nl::FoncComb::bread_gen(const FoncComb& parent, float regen_proba, float mut_proba, float mut_factor)
{
#ifdef NL_DEBUG
	if (parent.m_h != m_h || parent.m_w != m_w)
		debug_break(BadSizeArgument, "Impossible breading !");
#endif // NL_DEBUG

	uint64_t mut_crit = mut_proba * NL_RAND_MAX,
		regen_crit = regen_proba * NL_RAND_MAX,
		bread_crit = .5f * NL_RAND_MAX;
	for (int h = 0; h < m_h; ++h) {
		if (rd::next() < regen_crit)
			RandSumRangedLayer(h);
		else {
			if (rd::next() < bread_crit)
				memcpy(m_matrix + h * m_w, parent.m_matrix + h * m_w, m_w * sizeof(float));
			if (rd::next() < mut_crit)
				MutaSumRangedLayer(h, mut_factor);
		}
	}
}

void nl::FoncComb::eval(const Buffer& b_ent, Buffer& b_out) const
{

#ifdef NL_DEBUG
	if (b_ent.get_capacity() < m_w || b_out.get_capacity() < m_h)
		debug_break(BadSizeArgument, "Taille de matrices incompatibles !");
#endif

	b_out.set_to_zero(m_h);
	for (size_t i = 0; i < m_h; ++i) {
		size_t di = i * m_w;
		for (size_t j = 0; j < m_w; ++j)
			b_out[i] += m_matrix[di + j] * b_ent[j];
	}
}

size_t nl::FoncComb::get_width() const
{
	return m_w;
}

size_t nl::FoncComb::get_height() const
{
	return m_h;
}

float* nl::FoncComb::get_matrix()
{
	return m_matrix;
}

const float* nl::FoncComb::get_matrix() const
{
	return m_matrix;
}

void nl::FoncComb::RandSumRangedLayer(size_t h_layer)
{
	float* mat_layer = m_matrix + (h_layer * m_w);
	size_t wd = m_w - 1;

	std::vector<uint32_t> val{}; val.reserve(m_w);
	for (size_t i = 0; i < wd; ++i) {
		val.push_back(rd::next() % RAND_MOD);
	}
	val.push_back(RAND_MOD);
	std::sort(std::begin(val), std::end(val) - 1);

	uint32_t last = 0;
	for (size_t w = 0; w < m_w; ++w) {
		mat_layer[w] = (float)(val[w] - last) / RAND_MOD;
		last = val[w];
	}
}

void nl::FoncComb::MutaSumRangedLayer(size_t h_layer, float mut_factor)
{
	auto sig_square = [](float x) { if (x >= 0) return x * x; return -x * x; };
	float* mat_layer = m_matrix + (h_layer * m_w);
	uint64_t mut_crit = mut_factor * NL_RAND_MAX;
	float sum = 0;
	for (size_t w = 0; w < m_w; ++w) {
		if (rd::next() < mut_crit) {
			mat_layer[w] *= (float)(rd::next() % RAND_MOD) / RAND_MOD + .5f;
		}
		sum += mat_layer[w];
	}
	float rect_fact = 1.f / sum;
	for (size_t w = 0; w < m_w; ++w)
		mat_layer[w] *= rect_fact;
}
