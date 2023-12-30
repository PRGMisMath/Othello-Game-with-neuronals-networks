#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Othello.hpp"
extern "C" {
#include "NLNetwork.h"
}


struct GamePos { int x, y; };

class GPlayer {
public:
	virtual GamePos play(Othello game, Othello::Player current) = 0;
};

// Hold a copy of a network
class NLBot : GPlayer {
private:
	NLNetwork m_ia;

public:
	NLBot(NLNetwork& ia);
	~NLBot();

	virtual GamePos play(Othello game, Othello::Player current) override;
};


// Hold a pointer of a network
class NLWrapper : GPlayer {
private:
	NLNetwork* m_ia;

public:
	NLWrapper(NLNetwork& ia);
	void set_ia(NLNetwork& ia);

	virtual GamePos play(Othello game, Othello::Player current) override;
};


// This min_max bot only allow for 1 movement of look ahead. (Future amelioration)
class MinMaxBot : GPlayer {

public:
	virtual GamePos play(Othello game, Othello::Player current) override;
};

GamePos pred(NLNetwork& ia, const Othello& game, Othello::Player p);


#endif