#ifndef P_GENERATION_HPP
#define P_GENERATION_HPP


#include<vector>
#include "NLNetwork.hpp"
#include "Othello.hpp"
#include "Random.hpp"


// --- Paramètres --- //

#define HOW_BIG 80
#define NB_IA 50
#define NB_IA_PER_POOL 10
#define NB_POOL 5

#define REGEN_PROBA .01f
#define MUT_PROBA .03f
#define MUT_FACTOR .01f
#define BUFF_CAPA_MAX 64

const nl::Network::format layout[5] = {
	{64,64,nl::regres},
	{64,64,nl::regres},
	{64,32,nl::regres},
	{32,10,nl::regres},
	{10,1,nl::regres}
};

// ------------------ //


typedef std::vector<nl::Network*>::iterator refAI;

int pred(refAI ai, const Othello& game, Othello::Player p);
refAI run_pool(std::vector<refAI> pool);
refAI tournament(std::vector<std::vector<refAI>> pools);
refAI generation(std::vector<nl::Network*>& ias);


#endif