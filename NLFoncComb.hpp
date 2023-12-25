#ifndef NL_FONC_COMB_HPP
#define NL_FONC_COMB_HPP

#include "NLBuffer.hpp"

namespace nl {


class FoncComb {
public:

	FoncComb();

	FoncComb(size_t w, size_t h);

	~FoncComb();

	void init(size_t w, size_t h);

	void rand_gen();

	void bread_gen(const FoncComb& parent, float regen_proba, float mut_proba, float mut_factor);

	void eval(const Buffer& b_ent, Buffer& b_out) const;

	size_t get_width() const;

	size_t get_height() const;

	float* get_matrix();

	const float* get_matrix() const;

private:

	// void RandRangeLayer(size_t h_layer, float min_val, float max_val); // Not needed

	void RandSumRangedLayer(size_t h_layer);

	// void MutaRangeLayer(size_t h_layer, float min_val, float max_val); // Not needed

	void MutaSumRangedLayer(size_t h_layer, float mut_factor);

private:

	size_t m_h, m_w;

	/**
	* @brief Convention de stockage : (h,w) -> [h * m_w + w]
	*/
	float* m_matrix;

	friend class Network;
};


}


#endif