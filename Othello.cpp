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
	m_board(), m_canPlayOn(), m_playerView(),
	nb_piece(), nb_possibility()
{
	this->reset();
}

void Othello::reset()
{
	memset(m_board, 0, BOARD_SIZE * BOARD_SIZE);

	// Plateau au début du jeu //
	SetCaseDatA(InfoBoard::Noir, 3, 3); SetCaseDatA(InfoBoard::Blanc, 3, 4);
	SetCaseDatA(InfoBoard::Blanc, 4, 3); SetCaseDatA(InfoBoard::Noir, 4, 4);

	nb_piece[0] = 2; nb_piece[1] = 2;

	this->CountPossibilitY();
}

std::vector<Pos> Othello::play(Othello::Player p, int x, int y)
{
	assert(x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE); // Erreur dans l'implémentation 
	if (!m_canPlayOn[p-1][x][y])
		return {};
	
	std::vector<Pos> Dchange{}, Tchange{ {x,y} };
	Dchange.reserve(7);
	Tchange.reserve(28);
	for (int d = 0; d < 8; ++d) {
		int tX = x, tY = y;
		while (dirs[d](tX, tY))
			if (m_board[tX][tY] == p % 2 + 1)
				Dchange.push_back({ tX,tY });
			else if (m_board[tX][tY] == p && !Dchange.empty()) {
				// Submit changes
				for (const Pos& pos : Dchange) {
					SetCaseDatA((InfoBoard)p, pos.x, pos.y);
				}
				nb_piece[p-1] += Dchange.size();
				nb_piece[2-p] -= Dchange.size();
				Tchange.insert(std::end(Tchange), std::begin(Dchange), std::end(Dchange));
				break;
			}
			else break;
		Dchange.clear();
	}
	SetCaseDatA((InfoBoard)p, x, y);
	++nb_piece[p - 1];
	this->CountPossibilitY();

	return Tchange;
}

bool Othello::canPlay(Othello::Player p) const
{
	return (p != Othello::Player::Nobody) && nb_possibility[p-1] != 0;
}

bool Othello::canPlay(Othello::Player p, int x, int y) const
{
	return (p!=Othello::Player::Nobody) && m_canPlayOn[p-1][x][y];
}

bool* Othello::getPlayableSpot(Othello::Player p) const
{
	return (bool*)(this->m_canPlayOn[p-1]);
}

bool Othello::isFinish() const
{
	return nb_possibility[0] == 0 && nb_possibility[1] == 0;
}

int Othello::getScore(Othello::Player p) const
{
	return (p == Othello::Player::Nobody) ? 0 : nb_piece[p-1];
}

Othello::Player Othello::winner() const
{
	return (nb_piece[0] == nb_piece[1]) ? Othello::Player::Nobody : ((nb_piece[0] > nb_piece[1]) ? Othello::Player::PNoir : Othello::Player::PBlanc);
}

const char * Othello::getBoard() const
{
	return (const char*)m_board;
}

float* Othello::getPlayerView(Othello::Player p)
{
	return (p == Player::Nobody) ? NULL : (float*)(m_playerView[p-1]);
}

void Othello::SetCaseDatA(Othello::InfoBoard p, int x, int y)
{
	m_board[x][y] = p;
	if (p == Othello::None) {
		m_playerView[0][x][y] = 0;
		m_playerView[1][x][y] = 0;
	}
	else {
		m_playerView[0][x][y] = (p == Othello::Noir) * 2 - 1;
		m_playerView[1][x][y] = (p == Othello::Blanc) * 2 - 1;
	}
}

// Pas très optimisé
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
				if (this->IsPlayable((Othello::Player)(p+1), x, y)) {
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
