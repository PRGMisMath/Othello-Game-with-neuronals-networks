#ifndef NL_NETWORK_HPP
#define NL_NETWORK_HPP


#include "NLFoncComb.hpp"
#include "NLFoncActiv.hpp"
#include<vector>

namespace nl {

class Network {
public:
	// (w,h)
	struct format { size_t w, h; float (*act)(float); };

	Network(const format* sizes, size_t l_sizes, FoncActiv activ);

	~Network();

	void rand_gen();

	void bread_gen(const Network& parent, float regen_proba, float mut_proba, float mut_factor);

	void eval(const Buffer& in, Buffer& out, Buffer& process);

private:

	bool CheckCompatibility(const Network& other);

private:



	std::vector<FoncComb*> m_layers;

	FoncActiv m_activ;
};



}

#endif