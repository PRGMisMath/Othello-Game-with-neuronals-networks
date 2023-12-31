#ifndef IA_TRAINER_HPP
#define IA_TRAINER_HPP

#include<vector>
#include<iostream>
#include<thread>
#include<list>
#include "Player.hpp"
extern "C" {
#include "NLNetwork.h"
}


typedef std::vector<TrainAI>::iterator refAI;


struct TrainInfo {
	int nb_gen;
	// Add a gen info vector (later)
};

struct TrainParam {
	int nb_ias;
	int nb_pool;

	float regen_proba, hard_regen_proba, mut_proba, mut_factor;

	struct ScoreParam {
		int lose, draw, win;
		int bonus_winner;
		float mmbFact, refFact;
		int regenFact;
		float elim_adv_fact;
	} score;
};


class ThreadTrain {
private:

	Othello m_game;

	refAI m_winner;

	TrainParam::ScoreParam score_param;

private:

	int FightPlayeR(refAI ia, GPlayer* player);

public:
	ThreadTrain();
	ThreadTrain(const TrainParam::ScoreParam& param);
	void setParams(const TrainParam::ScoreParam& param);

	std::vector<refAI> thread_ias;
	std::list<GPlayer*>* p_refs;
	MinMaxBot* p_bot;

	void run_pool();
	void fight_bot();
	void fight_refs();

	refAI getWinner();
};


class IACoach {
private:
	
	std::vector<TrainAI> m_train_ias;

	TrainInfo m_info;

	TrainParam m_param;

	MinMaxBot m_bot;
	std::list<GPlayer*> m_refs;

	std::thread m_thread;

	std::vector<ThreadTrain> m_train_threads;

	refAI m_winner;

	bool runMinMax = true, runRef = true, runPool = true;

	friend void console_thread(IACoach* self);

public:

	bool isAIPlaying, isAIGenerating;

	IACoach(const TrainParam& param, NLFormat* format, size_t l_format);
	~IACoach();

	void launch_console();

	void join_console();

	void disp_info();

	void dispParamInfo();
	const TrainParam& getParams();

	void dispAIInfo();
	refAI getAI(size_t index);
	refAI getAIWinner();

	void dispRefInfo();
	size_t getNumRefs();
	GPlayer* giveRefPlayer(size_t index);

	void generation();
};


#endif // !IA_TRAINER_HPP
