#include "Othello.hpp"

#include<cstring>
#include<cassert>


bool DirN(int& x, int& y)
{
	return --x >= 0;
}
bool DirNE(int& x, int& y)
{
	return DirN(x, y) && DirE(x, y);
}
bool DirE(int& x, int& y)
{
	return ++y < BOARD_SIZE;
}
bool DirSE(int& x, int& y)
{
	return DirS(x, y) && DirE(x, y);
}
bool DirS(int& x, int& y)
{
	return ++x < BOARD_SIZE;
}
bool DirSW(int& x, int& y)
{
	return DirS(x, y) && DirW(x, y);
}
bool DirW(int& x, int& y)
{
	return --y >= 0;
}
bool DirNW(int& x, int& y)
{
	return DirN(x, y) && DirW(x, y);
}


Othello::Othello() :
	m_board(), m_canPlayOn(),
	nb_piece(), nb_possibility()
{
	this->reset();
}

void Othello::reset()
{
	memset(m_board, 0, BOARD_SIZE * BOARD_SIZE);

	// Plateau au debut du jeu //
	m_board[3][3] = InfoBoard::Noir; m_board[3][4] = InfoBoard::Blanc;
	m_board[4][3] = InfoBoard::Blanc; m_board[4][4] = InfoBoard::Noir;

	nb_piece[0] = 2; nb_piece[1] = 2;

	this->CountPossibilitY();
}

struct Pos { int x; int y; };
void Othello::play(Othello::Player p, int x, int y)
{
	assert(x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE); // Erreur dans l'implementation 
	if (!m_canPlayOn[p - 1][x][y])
		return;

	int case_change = 0;
	std::vector<Pos> Dchange{};
	Dchange.reserve(7);
	for (int d = 0; d < 8; ++d) {
		int tX = x, tY = y;
		while (dirs[d](tX, tY))
			if (m_board[tX][tY] == p % 2 + 1)
				Dchange.push_back({ tX,tY });
			else if (m_board[tX][tY] == p && !Dchange.empty()) {
				// Submit changes
				for (const Pos& pos : Dchange) {
					m_board[pos.x][pos.y] = (InfoBoard)p;
				}
				nb_piece[p - 1] += Dchange.size();
				nb_piece[2 - p] -= Dchange.size();
				break;
			}
			else break;
		Dchange.clear();
	}
	nb_piece[p - 1] += case_change;
	nb_piece[2 - p] -= case_change;

	m_board[x][y] = (InfoBoard)p;
	++nb_piece[p - 1];

	this->CountPossibilitY();
}

bool Othello::canPlay(Othello::Player p) const
{
	return (p != Othello::Player::Nobody) && nb_possibility[p - 1] != 0;
}

bool Othello::canPlay(Othello::Player p, int x, int y) const
{
	return (p != Othello::Player::Nobody) && m_canPlayOn[p - 1][x][y];
}

bool* Othello::getPlayableSpot(Othello::Player p) const
{
	return (bool*)(this->m_canPlayOn[p - 1]);
}

bool Othello::isFinish() const
{
	return nb_possibility[0] == 0 && nb_possibility[1] == 0;
}

int Othello::getScore(Othello::Player p) const
{
	return (p == Othello::Player::Nobody) ? 0 : nb_piece[p - 1];
}

Othello::Player Othello::winner() const
{
	return (nb_piece[0] == nb_piece[1]) ? Othello::Player::Nobody : ((nb_piece[0] > nb_piece[1]) ? Othello::Player::PNoir : Othello::Player::PBlanc);
}

const char* Othello::getBoard() const
{
	return (const char*)m_board;
}

std::vector<Othello::View>* Othello::getPlayerViews(Othello::Player p, size_t buff_capa) const
{
	std::vector<Othello::View>* views = new std::vector<Othello::View>();
	views->resize(nb_possibility[p - 1]);
	bool* free_play_id = (bool*)m_canPlayOn[p - 1]; int count = 0;
	for (int i = 0; i < nb_possibility[p-1]; ++i) {
		Othello::View& view = (*views)[i];
		view.data = (float*)malloc(buff_capa * sizeof(float));
		assert(view.data != NULL && "Bad Alloc (gPV)");
		for (int i = 0; i < BOARD_SIZE; ++i)
			for (int j = 0; j < BOARD_SIZE; ++j)
				view.data[i * BOARD_SIZE + j] = (m_board[i][j] == InfoBoard::None) ? 0.5f : (m_board[i][j] == p);
		while (!*free_play_id) {
			++free_play_id;
			++count;
		}
		view.data[count] = p;
		view.play = count;
		UpdateBoardPlaY(view.data, p, count / BOARD_SIZE, count % BOARD_SIZE);
		++count; ++free_play_id;
	}


	return views;
}

int Othello::UpdateBoardPlaY(float* board, Othello::Player p, int x, int y) const
{
	int case_change = 0;
	std::vector<Pos> Dchange{};
	Dchange.reserve(7);
	for (int d = 0; d < 8; ++d) {
		int tX = x, tY = y;
		while (dirs[d](tX, tY))
			if (board[tX * BOARD_SIZE + tY] == 0.f)
				Dchange.push_back({ tX,tY });
			else if (board[tX * BOARD_SIZE + tY] == 1.f && !Dchange.empty()) {
				// Effectue les changements
				for (const Pos& pos : Dchange) {
					board[pos.x * BOARD_SIZE + pos.y] = 1.f;
				}
				case_change += Dchange.size();
				break;
			}
			else break;
		Dchange.clear();
	}
	return case_change;
}

// Pas tres optimal
void Othello::CountPossibilitY()
{
	memset(m_canPlayOn, false, BOARD_SIZE * BOARD_SIZE * NB_PLAYER);
	for (int p = 0; p < 2; ++p)
		nb_possibility[p] = 0;

	if (nb_piece[0] == 0 || nb_piece[1] == 0 || nb_piece[0] + nb_piece[1] == BOARD_SIZE * BOARD_SIZE)
		return;

	for (int p = 0; p < 2; ++p)
		for (int x = 0; x < BOARD_SIZE; ++x)
			for (int y = 0; y < BOARD_SIZE; ++y)
				if (this->IsPlayable((Othello::Player)(p + 1), x, y)) {
					++nb_possibility[p];
					m_canPlayOn[p][x][y] = true;
				}
}

bool Othello::IsPlayable(Othello::Player p, int x, int y) const
{
	if (m_board[x][y] != InfoBoard::None || p == Othello::Player::Nobody)
		return false;

	for (int d = 0; d < 8; ++d) {
		int tX = x, tY = y;
		bool as_white = false;
		while (dirs[d](tX, tY))
			if (m_board[tX][tY] == p % 2 + 1)
				as_white = true;
			else if (m_board[tX][tY] == p && as_white)
				return true;
			else break;
	}

	return false;
}
