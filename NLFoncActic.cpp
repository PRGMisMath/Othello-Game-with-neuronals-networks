#include "NLFoncActiv.hpp"

#include "NLDebug.hpp"
#include<cmath>

nl::FoncActiv::FoncActiv() :
	m_func()
{
}

nl::FoncActiv::FoncActiv(float(*func)(float)) :
	m_func(func)
{
}

void nl::FoncActiv::eval(const Buffer& b_ent, Buffer& b_out, size_t b_size) const
{
#ifdef NL_DEBUG
	if (b_ent.get_capacity() < b_size || b_out.get_capacity() < b_size)
		debug_break(BadSizeArgument, "Tailles differentes !");
#endif // NL_DEBUG
	for (int i = 0; i < b_size; ++i)
		b_out[i] = m_func(b_ent[i]);
}

void nl::FoncActiv::set_func(float(*func)(float))
{
	m_func = func;
}

float nl::trivalactiv(float x)
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

float nl::bivalactiv(float x)
{
	float dec = 0.5f - x;
	dec = 1 / (1 + expf(8 * dec));
	if (dec < 0.1f)
		return 0.f;
	if (dec > 0.9f)
		return 1.f;
	return dec;
}

float nl::regres(float x)
{
	return 1/(1+expf(8*(.5f-x)));
}
