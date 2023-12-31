#include "Player.hpp"
#include<cassert>


GamePos pred(NLNetwork& ia, const Othello& game, Othello::Player p) {
	size_t max_buff_capa = ia.max_dim;
	float max_val = 0.f; int best_play = -1;
	float* process = (float*)malloc(ia.max_dim * sizeof(float));
	assert(process);
	std::vector<Othello::View>* p_views = game.getPlayerViews(p, max_buff_capa);
	for (Othello::View& view : *p_views) {
		eval_network(&ia, view.data, process, view.data);
		if (view.data[0] > max_val) {
			max_val = view.data[0];
			best_play = view.play;
		}
	}
	for (auto v : *p_views)
		free(v.data);
	delete p_views;
	free(process);
	return { best_play / BOARD_SIZE, best_play % BOARD_SIZE };
}

NLBot::NLBot(NLNetwork& ia) :
	m_ia()
{
	create_copy_network(&m_ia, &ia);
}

NLBot::~NLBot()
{
	destroy_network(&m_ia);
}

GamePos NLBot::play(Othello game, Othello::Player current)
{
    return pred(m_ia, game, current);
}

GamePos MinMaxBot::play(Othello game, Othello::Player current)
{
	float max_val = 0.f; int best_play = -1;
	float sum;
	std::vector<Othello::View>* p_views = game.getPlayerViews(current, BOARD_SIZE * BOARD_SIZE);
	for (Othello::View& view : *p_views) {
		sum = 0;
		for (int j = 0; j < BOARD_SIZE * BOARD_SIZE; ++j)
			sum += view.data[j];
		if (sum > max_val) {
			max_val = sum;
			best_play = view.play;
		}
	}
	for (auto v : *p_views)
		free(v.data);
	delete p_views;
	return { best_play / BOARD_SIZE, best_play % BOARD_SIZE };
}

NLWrapper::NLWrapper(NLNetwork& ia) :
	m_ia(&ia)
{
}

void NLWrapper::set_ia(NLNetwork& ia)
{
	m_ia = &ia;
}

GamePos NLWrapper::play(Othello game, Othello::Player current)
{
	return pred(*m_ia, game, current);
}
