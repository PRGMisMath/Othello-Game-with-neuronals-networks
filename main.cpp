//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<iostream>
#include<string>
#include<thread>
#include "Othello.hpp"
#include "GameView.hpp"
extern "C" {
#include "NLNetwork.h"
#include "Random.h"
}
#include "IATrainer.hpp"


// --- Constantes --- //

#define HOW_BIG 80

NLFormat netw_format[5]{
	{64,512,nl_regres},
	{512,512,nl_bivalactiv},
	{512,32,nl_regres},
	{32,10,nl_regres},
	{10,1,[](float x) -> float { return x; } }
};

TrainParam param{
	/* nb_ias */ 100,
	/* nb_pool */ 10,

	/* regen_proba */ .04f,
	/* hard_regen_proba */ .90f,
	/* mut_proba */ .14f,
	/* mut_factor */ .8f,

	/* ScoreParam */
	{
		/* lose */ 0, /* draw */ 50, /* win */ 300,
		/* bonus_winner */ 1000,
		/* mmbFact */ 10.f, /* refFact */ 2.f,
		/* regenFact */ 10,
		/* elim_adv_fact */ 2.f
	}
};



// ------------------ //



sf::Texture texPion{};
sf::Font gFont{};

const GameAppear GAOthello = {
	{8,8},
	2,
	&texPion,
	{20,20}
};


struct {
	uint8_t gameMode = 0; // 0x00 : 2 joueurs // 0x01 : joueur VS IA
	int iaAnswerDelay = 0; // Delai en nombre de frame (1 frame = 0,02 secondes)
	int nbGenIA = 0;
} gameState;


bool renderPlay(Othello& game, GameView& render, Othello::Player p, int x, int y) {
	if (!game.canPlay(p, x, y))
		return false;
	game.play(p, x, y);
	render.update();
	return true;
}

std::string getGameInfo(Othello& game) {
	if (!game.isFinish())
		switch (gameState.gameMode)
		{
		case 0x00:
			return "Noir : " + std::to_string(game.getScore(Othello::PNoir)) + " --- Blanc : " + std::to_string(game.getScore(Othello::PBlanc));
		case 0x01:
			return "Joueur (Noir) : " + std::to_string(game.getScore(Othello::PNoir)) + " --- IA (Blanc) : " + std::to_string(game.getScore(Othello::PBlanc));
		case 0x02:
			return "IA : " + std::to_string(game.getScore(Othello::PNoir)) + " --- Min/Max : " + std::to_string(game.getScore(Othello::PBlanc));
		}
	else
		switch (game.winner())
		{
		case Othello::PNoir:
			return ((gameState.gameMode == 0x00) ? "Victoire des Noirs (" : "Victoire (") + std::to_string(game.getScore(Othello::PNoir)) + " contre " + std::to_string(game.getScore(Othello::PBlanc)) + ") !";
		case Othello::PBlanc:
			return ((gameState.gameMode == 0x00) ? "Victoire des Blancs (" : "Défaite (") + std::to_string(game.getScore(Othello::PBlanc)) + " contre " + std::to_string(game.getScore(Othello::PNoir)) + ") !";
		case Othello::Nobody:
			return "Egalite !";
		}
	return "Unknown gamemode !";
}



int main() {

	IACoach coach{ param, netw_format, 5};
	coach.launch_console();

	// --- Graphics --- //

	texPion.loadFromFile("ressource\\pion.png");
	gFont.loadFromFile("ressource\\arial.ttf");

	sf::RenderWindow wndw{ sf::VideoMode(HOW_BIG * 10,HOW_BIG * 10),"Othello" };
	wndw.setFramerateLimit(50);

	Othello Ghandler{};
	GameView Grender{ GAOthello, Ghandler.getBoard(), HOW_BIG };
	Grender.setPosition({ HOW_BIG,HOW_BIG });
	sf::RectangleShape PIndic{ {70,70} };
	PIndic.setPosition({ 5,5 });
	PIndic.setTexture(&texPion);
	PIndic.setTextureRect({ 0,0,20,20 });
	sf::Text info{ "Noir : 2 --- Blanc : 2",gFont };
	info.setPosition({ 300,7 });

	// --- Game States --- //
	bool isStarted = false;
	bool playChoosen = false;
	bool playerControl = true; // Les joueurs peuvent-ils interagir avec le plateau ? (Faux <=> IA Cooldown <=> [Multijoueur local])
	int iaAnswerCooldown = -1; // -1 : c'est au joueur de jouer
	GamePos play;
	Othello::Player current = Othello::PNoir;

	NLWrapper* NIA = new NLWrapper(coach.getAI(0)->network);
	MinMaxBot* BIA = new MinMaxBot();


	while (wndw.isOpen()) {
		sf::Event event;
		while (wndw.pollEvent(event))
			switch (event.type)
			{
			case sf::Event::Closed:
				wndw.close();
				break;
			case sf::Event::MouseButtonPressed:
				if (playerControl && event.mouseButton.x >= HOW_BIG && event.mouseButton.x < HOW_BIG * 9 && event.mouseButton.y >= HOW_BIG && event.mouseButton.y < HOW_BIG * 9 && !Ghandler.isFinish()) {
					playChoosen = true;
					play.x = (event.mouseButton.y - HOW_BIG) / HOW_BIG;
					play.y = (event.mouseButton.x - HOW_BIG) / HOW_BIG;
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::R: // Relance la partie depuis 0
					dynamic_cast<NLWrapper*>(NIA)->set_ia(coach.getAIWinner()->network);
					Ghandler.reset();
					Grender.update();
					isStarted = false;
					playerControl = true;
					iaAnswerCooldown = -1;
					current = Othello::PNoir;
					if (gameState.gameMode == 0x02) {
						playerControl = false;
						iaAnswerCooldown = gameState.iaAnswerDelay;
					}
					PIndic.setTextureRect({ 20 * (current - 1), 0, 20 * current, 20 });
					info.setString(getGameInfo(Ghandler));
					break;
				case sf::Keyboard::M: // Change de mode de jeu (multi ou ia)
					if (!isStarted && !coach.isAIGenerating) {
						gameState.gameMode = (gameState.gameMode + 1) % 3;
						switch (gameState.gameMode) // Switch sur le nouveau mode de jeu
						{
						case 0x00: // Human VS Human
							coach.isAIPlaying = false;
							break;
						case 0x01: // Human VS Bot
							coach.isAIPlaying = true;
							break;
						case 0x02: // Bot VS Bot
							coach.isAIPlaying = true;
							playerControl = false;
							iaAnswerCooldown = gameState.iaAnswerDelay;
							break;
						}
						info.setString(getGameInfo(Ghandler));
					}
					break;
				}
				break;
			}

		// --- Gestion de l'IA --- //
		if (gameState.gameMode == 0x01) {
			if (iaAnswerCooldown == 0) {
				playChoosen = true;
				play = NIA->play(Ghandler, current);
			}
			if (iaAnswerCooldown >= 0)
				--iaAnswerCooldown;
		}
		if (gameState.gameMode == 0x02) {
			if (iaAnswerCooldown == 0) {
				playChoosen = true;
				if (current == Othello::PBlanc) play = BIA->play(Ghandler, current);
				if (current == Othello::PNoir) play = NIA->play(Ghandler, current);
			}
			if (iaAnswerCooldown >= 0)
				--iaAnswerCooldown;
		}
		// ----------------------- //
		// 
		// --- Gestion des tours de jeux --- //
		if (playChoosen) { // Si le choix du coup a ete effectue
			isStarted = true;
			playChoosen = false;
			if (renderPlay(Ghandler, Grender, current, play.x, play.y)) { // Si le coup est valide
				info.setString(getGameInfo(Ghandler));
				if (Ghandler.canPlay((Othello::Player)(current % 2 + 1))) {
					current = (Othello::Player)(current % 2 + 1);
					if (gameState.gameMode == 0x01) {
						playerControl = !playerControl;
					}
					PIndic.setTextureRect({ 20 * (current - 1), 0, 20, 20 });
				}
				if (!playerControl && gameState.gameMode != 0x00 && !Ghandler.isFinish())
					iaAnswerCooldown = gameState.iaAnswerDelay;
			}
			if (Ghandler.isFinish()) {
				isStarted = false;
				coach.isAIPlaying = false;
			}
		}
		// --------------------------------- //

		wndw.clear(sf::Color(200, 200, 200));
		wndw.draw(Grender);
		wndw.draw(PIndic);
		wndw.draw(info);
		wndw.display();
	}

	coach.join_console();

	delete NIA;
	delete BIA;

	return 0;
}