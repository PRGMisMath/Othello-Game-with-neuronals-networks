#include "IATrainer.hpp"

#include<string>
#include<ctime>
extern "C" {
#include "Random.h"
}



//////////////////////////
//  --- ThreadTrain --- //
//////////////////////////

int ThreadTrain::FightPlayeR(refAI ia, GPlayer* player)
{
	int score = 0;
	Othello::Player current = Othello::PNoir;
	m_game.reset();
	while (!m_game.isFinish()) {
		if (m_game.canPlay(current)) {
			GamePos coup;
			if (current == Othello::PNoir) {
				coup = pred((*ia).network, m_game, current);
			}
			else {
				coup = player->play(m_game, current);
			}
			m_game.play(current, coup.x, coup.y);
		}
		current = (Othello::Player)(current % 2 + 1);
	}
	if (m_game.winner() == Othello::Nobody)
		score = score_param.draw + m_game.getScore(Othello::PNoir);
	else if (m_game.winner() == Othello::PNoir)
		score = score_param.win + m_game.getScore(Othello::PNoir);
	else
		score = score_param.lose + m_game.getScore(Othello::PNoir);
	if (m_game.getScore(Othello::PBlanc) == 0)
		score += score_param.elim_adv_fact * (BOARD_SIZE * BOARD_SIZE - m_game.getScore(Othello::PNoir));
	return score;
}

ThreadTrain::ThreadTrain() :
	m_game(), m_winner(), score_param()
{
}

ThreadTrain::ThreadTrain(const TrainParam::ScoreParam& param) :
	m_game(), m_winner(), score_param(param)
{
}

void ThreadTrain::setParams(const TrainParam::ScoreParam& param)
{
	score_param = param;
}

void ThreadTrain::run_pool()
{
	size_t n = thread_ias.size();
	Othello::Player current = Othello::PNoir;

	for (int p1 = 0; p1 < n; ++p1) {

		// IAS
		for (int p2 = p1 + 1; p2 < n; ++p2) {
			m_game.reset();

			while (!m_game.isFinish()) {
				if (m_game.canPlay(current)) {
					GamePos coup = pred((*thread_ias[p1]).network, m_game, current);
					m_game.play(current, coup.x, coup.y);
				}
				current = (Othello::Player)(current % 2 + 1);
			}
			if (m_game.winner() == Othello::Nobody) {
				thread_ias[p1]->comp_score += score_param.draw;
				thread_ias[p2]->comp_score += score_param.draw;
			}
			else {
				thread_ias[(m_game.winner() == Othello::PNoir) ? p1 : p2]->comp_score += score_param.win;
				thread_ias[(m_game.winner() == Othello::PNoir) ? p2 : p1]->comp_score += score_param.lose;
			}
			thread_ias[p1]->comp_score += m_game.getScore(Othello::PNoir);
			thread_ias[p2]->comp_score += m_game.getScore(Othello::PBlanc);
		}

	}

	int winner = 0;
	for (int p = 0; p < n; ++p) {
		if (thread_ias[p]->comp_score > thread_ias[winner]->comp_score)
			winner = p;
	}
	m_winner = thread_ias[winner];
	m_winner->comp_score += score_param.bonus_winner;
}

void ThreadTrain::fight_bot()
{
	size_t n = thread_ias.size();
	for (int p = 0; p < n; ++p) {
		thread_ias[p]->ref_score += score_param.mmbFact * FightPlayeR(thread_ias[p], (GPlayer*)p_bot);
	}
}

void ThreadTrain::fight_refs()
{
	size_t n = thread_ias.size();
	for (int p = 0; p < n; ++p) {
		for (auto ref : *p_refs)
			thread_ias[p]->ref_score += score_param.refFact * FightPlayeR(thread_ias[p], ref);
	}
}

refAI ThreadTrain::getWinner()
{
	return m_winner;
}




/////////////////////
// --- IACoach --- //
/////////////////////

IACoach::IACoach(const TrainParam& param, NLFormat* format, size_t l_format) :
	m_param(param), m_info(),
	m_refs(), m_train_ias(),
	m_bot(),
	m_train_threads()
{
	rd_set_seed(std::time(0));

	m_train_ias.resize(param.nb_ias);
	for (size_t s = 0; s < param.nb_ias; ++s) {
		m_train_ias[s].comp_score = 0;
		m_train_ias[s].ref_score = 0;
		create_rand_network(&(m_train_ias[s].network), format, l_format);
	}

	m_train_threads.resize(param.nb_pool);
	for (int i = 0; i < param.nb_pool; ++i) {
		m_train_threads[i].setParams(param.score);
		m_train_threads[i].thread_ias = std::vector<refAI>();
		m_train_threads[i].p_refs = &m_refs;
		m_train_threads[i].p_bot = &m_bot;
	}

	m_winner = std::begin(m_train_ias);
}

IACoach::~IACoach()
{
	for (GPlayer* ref : m_refs)
		delete ref;
	for (TrainAI ai : m_train_ias)
		destroy_network(&ai.network);
}


// - Generation - //

void run_pool(ThreadTrain* tt) {
	tt->run_pool();
}

void run_minmax(ThreadTrain* tt) {
	tt->fight_bot();
}

void run_refs(ThreadTrain* tt) {
	tt->fight_refs();
}

refAI tournament(std::vector<ThreadTrain>& pools) {
	std::vector<refAI> pool_win{};
	std::vector<std::thread> threads{};
	pool_win.reserve(pools.size());
	threads.reserve(pools.size());
	int pool_count = 0;
	for (auto& pool : pools) {
		threads.push_back(std::thread(run_pool, &pool));
	}
	for (auto& t : threads) t.join();
	pools[0].thread_ias.clear();
	for (auto& pool : pools) {
		pools[0].thread_ias.push_back(pool.getWinner());
	}
	pools[0].run_pool();
	return pools[0].getWinner();
}

void IACoach::generation() {
	int nb_ia_pool = m_param.nb_ias / m_param.nb_pool,
		nb_pool = m_param.nb_pool;

	for (int i = 0; i < m_param.nb_pool; ++i)
		m_train_threads[i].thread_ias.clear();
	int ind;
	for (refAI ai = std::begin(m_train_ias); ai != std::end(m_train_ias); ++ai) {
		ai->comp_score = 0; ai->ref_score = 0;
		ind = rd_next() % nb_pool;
		while (m_train_threads[ind].thread_ias.size() == nb_ia_pool) ind = (ind + 1) % nb_pool;
		m_train_threads[ind].thread_ias.push_back(ai);
	}

	if (runMinMax) {
		std::vector<std::thread> threads{};
		threads.reserve(m_train_threads.size());
		int pool_count = 0;
		for (auto& pool : m_train_threads) {
			threads.push_back(std::thread(run_minmax, &pool));
		}
		for (auto& t : threads) t.join();
	}
	if (runRef) {
		std::vector<std::thread> threads{};
		threads.reserve(m_train_threads.size());
		int pool_count = 0;
		for (auto& pool : m_train_threads) {
			threads.push_back(std::thread(run_refs, &pool));
		}
		for (auto& t : threads) t.join();
	}
	if (runPool)
		m_winner = tournament(m_train_threads);

	int max_score = 0;
	for (refAI ai = std::begin(m_train_ias); ai != std::end(m_train_ias); ++ai) {
		if (max_score < ai->comp_score + ai->ref_score) {
			max_score = ai->comp_score + ai->ref_score;
			m_winner = ai;
		}
	}
	max_score /= m_param.score.regenFact;
	for (refAI ia = std::begin(m_train_ias); ia != std::end(m_train_ias); ++ia) {
		if (ia._Ptr != m_winner._Ptr) {
			mix_network(&(*ia).network, &(*m_winner).network,
				((ia->comp_score + ia->ref_score < max_score) ? m_param.hard_regen_proba : m_param.regen_proba),
				m_param.mut_proba, m_param.mut_factor);
		}
	}
}


// - Console - //

void chunk(std::string& phrase)
{
	while (phrase.size() != 0 && isspace(phrase.back()))
		phrase.pop_back();
	while (phrase.size() != 0 && isspace(phrase.front()))
		phrase.erase(std::begin(phrase));
}

std::vector<std::string> cut_in(std::string phrase) {
	std::vector<std::string> result{ "" };
	bool quote = false;
	for (char i : phrase) {
		if (isspace(i) && !quote) {
			if (result.back() != "")
				result.push_back("");
		}
		else if (i == '"')
			quote = !quote;
		else
			result.back().push_back(i);
	}
	if (quote)
		throw std::runtime_error("Guillemet non fermant !");
	return result;
}

bool errorno(const char* message, bool condition = false)
{
	if (condition)
		return false;
	std::clog << "Erreur : " << message;

	return true;
}

void console_thread(IACoach* self) {
	int nb_gen = 0;


	std::string entree;
	std::vector<std::string> cmd;
	while (true) {
		std::cout << "> ";
		std::getline(std::cin, entree);
		chunk(entree);
		if (self->isAIPlaying)
			std::cout << "Finis ta partie pour utiliser l'invite !";
		if (entree == "")
			continue;
		if (entree == "quit")
			break;
		try
		{
			cmd = cut_in(entree);
		}
		catch (const std::exception& e)
		{
			errorno(e.what());
			std::cout << std::endl;
			continue;
		}


		if (cmd[0] == "generation") {
			self->isAIGenerating = true;
			if (cmd.size() == 1) {
				std::cout << "Generation : " << ++nb_gen << "\n";
				self->generation();
			}
			else if (cmd.size() == 2) try {
				int boucle = std::stoi(cmd[1]);
				for (int i = 0; i < boucle; ++i) {
					std::cout << "Generation : " << ++nb_gen << "\n";
					self->generation();
				}
			}
			catch (const std::exception& e) {
				errorno("Argument invalide !");
				std::cout << std::endl;
				continue;
			}
			else
				errorno("Trop d'arguments !");
			self->isAIGenerating = false;
		}
		else if (cmd[0] == "store") {
			if (cmd.size() > 1) {
				errorno("Trop d'arguments !");
				continue;
			}
			self->m_refs.push_back((GPlayer*)(new NLBot((*self->m_winner).network)));
			std::cout << "Nouvelle IA ajoutee comme reference !\n";
		}
		else if (cmd[0] == "unstore") {
			if (cmd.size() > 1) {
				errorno("Trop d'arguments !");
				continue;
			}
			if (self->m_refs.size() == 1) {
				errorno("Pas d'IA a enlever !");
				continue;
			}
			delete self->m_refs.front();
			self->m_refs.pop_front();
			std::cout << "IA enlevé !";
		}
		else if (cmd[0] == "info") {
			if (cmd.size() > 1) {
				errorno("Trop d'arguments !");
				continue;
			}
			self->disp_info();
			// Ajouter disp_info ; dispRefInfo ; dispParamInfo
		}
		else if (cmd[0] == "mode") {
			if (cmd.size() == 1) {
				std::cout << "Classement par pool : " << self->runPool
					<< "\nClassement par rapport a un algo min/max : " << self->runMinMax
					<< "\nClassement par rapport aux ias de reference : " << self->runRef
					<< '\n';
				continue;
			}
			if (cmd.size() > 2) {
				errorno("Trop d'arguments !");
				continue;
			}
			if (cmd[1] == "minmax") {
				self->runPool = false;
				self->runMinMax = true;
				self->runRef = false;
			}
			else if (cmd[1] == "all") {
				self->runPool = true;
				self->runMinMax = true;
				self->runRef = true;
			}
			else if (cmd[1] == "pool") {
				self->runPool = true;
				self->runMinMax = false;
				self->runRef = false;
			}
			else
				errorno("Parametre inexistant !");
		}
		else
			errorno("Commande non reconnue !");


		std::cout << std::endl;
	}
}



void IACoach::launch_console()
{
	m_thread = std::thread(console_thread, this);
}

void IACoach::join_console()
{
	m_thread.join();
}

void IACoach::disp_info()
{
	dispAIInfo();
	dispRefInfo();
	dispParamInfo();
}

void IACoach::dispParamInfo()
{
	// Pour plus tard
}

const TrainParam& IACoach::getParams()
{
	return m_param;
}

void IACoach::dispAIInfo()
{
	std::cout << "Nombre d'IA : " << m_param.nb_ias << "\nGeneration n" << m_info.nb_gen << "\n";
}

refAI IACoach::getAI(size_t index)
{
	return std::begin(m_train_ias) + index;
}

refAI IACoach::getAIWinner()
{
	return m_winner;
}

void IACoach::dispRefInfo()
{
	std::cout << "Il y a " << m_refs.size() << " IA de references.\n";
}

size_t IACoach::getNumRefs()
{
	return m_refs.size();
}

GPlayer* IACoach::giveRefPlayer(size_t index)
{
	auto pointeur = std::begin(m_refs);
	for (int i = 0; i < index; ++i)
		++pointeur;
	return *pointeur;
}
