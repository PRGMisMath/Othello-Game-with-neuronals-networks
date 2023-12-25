#include "NLNetwork.hpp"

nl::Network::Network(const format* sizes, size_t l_sizes, FoncActiv activ) :
	m_layers(),m_activ(activ)
{
	m_layers.reserve(l_sizes);
	for (size_t s = 0; s < l_sizes; ++s) {
		m_layers.push_back(new FoncComb(sizes[s].w,sizes[s].h));
	}
}

nl::Network::~Network()
{
	for (FoncComb* lay : m_layers)
		delete lay;
}

void nl::Network::rand_gen()
{
	for (FoncComb* lay : m_layers)
		lay->rand_gen();
}

void nl::Network::bread_gen(const Network& parent, float regen_proba, float mut_proba, float mut_factor)
{
	for (size_t s = 0; s < std::size(m_layers); ++s)
		m_layers[s]->bread_gen(*parent.m_layers[s], regen_proba, mut_proba, mut_factor);
}

void nl::Network::eval(const Buffer& in, Buffer& out, Buffer& process)
{
	const Buffer* entry = &in;
	for (const FoncComb* lay : m_layers) {
		lay->eval(*entry, process);
		m_activ.eval(process, out, lay->get_height());
		entry = &out;
	}

}

bool nl::Network::CheckCompatibility(const nl::Network& other)
{
	if (std::size(m_layers) != std::size(other.m_layers))
		return false;
	for (size_t s = 0; s < std::size(m_layers); ++s)
		if (m_layers[s]->m_h != other.m_layers[s]->m_h
			&& m_layers[s]->m_w != other.m_layers[s]->m_w)
			return false;
	return true;
}
