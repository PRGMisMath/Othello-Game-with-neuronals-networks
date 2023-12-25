#include "generation.hpp"
#include<iostream>



int pred(refAI ai, const Othello& game, Othello::Player p) {
	float max_val = 0.f; int best_play = -1;
	nl::Buffer process{ BUFF_CAPA_MAX };
	std::vector<Othello::View> p_views = game.getPlayerViews(p, BUFF_CAPA_MAX);
	for (Othello::View& view : p_views) {
		(*ai)->eval(*view.buff, *view.buff, process);
		if ((*view.buff)[0] > max_val) {
			max_val = (*view.buff)[0];
			best_play = view.play;
		}
	}
	for (auto v : p_views)
		delete v.buff;
	return best_play;
}

refAI run_pool(std::vector<refAI> pool) {
	size_t n = pool.size();
	Othello game{};
	Othello::Player current = Othello::PNoir;
	std::vector<int> score{};
	score.reserve(n);
	for (int i = 0; i < n; ++i) score.push_back(0);

	for (int p1 = 0; p1 < n; ++p1) {
		for (int p2 = p1 + 1; p2 < n; ++p2) {
			game.reset();

			while (!game.isFinish()) {
				if (game.canPlay(current)) {
					int coup = pred(pool[p1], game, current);
					game.play(current, coup / 8, coup % 8); 
				}
				current = (Othello::Player)(current % 2 + 1);
			}
			if (game.winner() == Othello::Nobody) {
				score[p1] += 100;
				score[p2] += 100;
			}
			else
				score[(game.winner() == Othello::PNoir) ? p1 : p2] += 300;
			score[p1] += game.getScore(Othello::PNoir);
			score[p2] += game.getScore(Othello::PBlanc);
		}
	}
	int winner = 0;
	std::cout << "\t\tScore | ";
	for (int p = 0; p < n; ++p) {
		std::cout << score[p] << " | ";
		if (score[p] > score[winner])
			winner = p;
	}
	std::cout << "\n\t\tGagnant : 0x" << reinterpret_cast<unsigned long long>(pool[winner]._Ptr) << '\n';
	return pool[winner];
}

refAI tournament(std::vector<std::vector<refAI>> pools) {
	std::vector<refAI> pool_win{};
	pool_win.reserve(NB_POOL);
	int pool_count = 0;
	for (std::vector<refAI> pool : pools) {
		std::cout << "\tPool " << ++pool_count << '\n';
		pool_win.push_back(run_pool(pool));
	}
	std::cout << "\tWinner bracket :\n";
	return run_pool(pool_win);
}

refAI generation(std::vector<nl::Network*>& ias) {
	std::vector<std::vector<refAI>> pools{};
	pools.reserve(NB_POOL);
	for (int i = 0; i < NB_POOL; ++i) {
		pools.push_back(std::vector<refAI>());
		pools[i].reserve(NB_IA_PER_POOL);
	}
	int ind;
	for (refAI ai = std::begin(ias); ai != std::end(ias); ++ai) {
		ind = rd::next() % NB_POOL;
		while (pools[ind].size() == NB_IA_PER_POOL) ind = (ind + 1) % NB_POOL;
		pools[ind].push_back(ai);
	}
	refAI winner = tournament(pools);
	for (refAI ia = std::begin(ias); ia != std::end(ias); ++ia) {
		if (ia != winner)
			(*ia)->bread_gen(**winner, REGEN_PROBA, MUT_PROBA, MUT_FACTOR);
	}
	return winner;
}
