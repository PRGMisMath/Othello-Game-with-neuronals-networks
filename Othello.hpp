#ifndef OTHELLO_HPP
#define OTHELLO_HPP

#include<vector>

#define BOARD_SIZE 8
#define NB_PLAYER 2

bool DirN(int& x, int& y);
bool DirNE(int& x, int& y);
bool DirE(int& x, int& y);
bool DirSE(int& x, int& y);
bool DirS(int& x, int& y);
bool DirSW(int& x, int& y);
bool DirW(int& x, int& y);
bool DirNW(int& x, int& y);

static constexpr bool (*dirs[8])(int&, int&) = { DirN,DirNE,DirE,DirSE,DirS,DirSW,DirW,DirNW };



class Othello {
public:
	enum InfoBoard : char {
		None = 0,
		Noir = 1,
		Blanc = 2
	};
	enum Player : char {
		Nobody = 0,
		PNoir = 1,
		PBlanc = 2
	};
	struct View {
		float* data;
		int play;
	};
public:
	Othello();
	void reset();
	void play(Othello::Player p, int x, int y);
	bool canPlay(Othello::Player p) const;
	bool canPlay(Othello::Player p, int x, int y) const;
	bool* getPlayableSpot(Othello::Player p) const;
	bool isFinish() const;
	int getScore(Othello::Player p) const;
	Othello::Player winner() const;
	const char* getBoard() const;
	/// Warning : disallocation of the views (float* arrays + vector) needed
	std::vector<View>* getPlayerViews(Othello::Player p, size_t buff_capa) const;
private:
	int UpdateBoardPlaY(float* board, Othello::Player p, int x, int y) const; // Retourne le nombre de cases qui ont changées de couleur
	void CountPossibilitY();
	bool IsPlayable(Othello::Player p, int x, int y) const;
private:
	InfoBoard m_board[BOARD_SIZE][BOARD_SIZE]; // Plateau[x:Horizontal][y:Vertical]
	bool m_canPlayOn[NB_PLAYER][BOARD_SIZE][BOARD_SIZE];
	int nb_possibility[NB_PLAYER];
	int nb_piece[NB_PLAYER]; // Joueurs : [0] = Noir & [1] = Blanc
};

#endif