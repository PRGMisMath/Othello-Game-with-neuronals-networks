#ifndef NL_FONC_ACTIV_HPP
#define NL_FONC_ACTIV_HPP

#include "NLBuffer.hpp"

namespace nl {

class FoncActiv {
public:
	FoncActiv();

	FoncActiv(float (*func)(float));

	void eval(const Buffer& b_ent, Buffer& b_out, size_t b_size) const;

	void set_func(float (*func)(float));

private:
	float (*m_func)(float);
};

float trivalactiv(float x);

float bivalactiv(float x);

float regres(float x);

}

#endif // !NL_FONC_ACTIV_HPP
