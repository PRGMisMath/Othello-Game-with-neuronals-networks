//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<iostream>
#include<string>
#include<thread>
#include "Othello.hpp"
#include "GameView.hpp"
#include "NeuronalNetwork.hpp"

#define HOW_BIG 80
#define NB_IA 100
#define NB_IA_PER_POOL 10

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
	int iaAnswerDelay = 50; // Délai en nombre de frame (1 frame = 0,02 secondes)
	int nbGenIA = 0;
} gameState;


// --- IA Stuff --- //

NeuronalNetwork* ias[NB_IA];

NeuronalNetwork* runPool(NeuronalNetwork** ia) {
	Othello game{};
	Othello::Player current = Othello::PNoir;
	NeuronalNetwork* oponents[2];
	int score[NB_IA_PER_POOL]{};

	for (int p1 = 0; p1 < NB_IA_PER_POOL - 1; ++p1) {
		oponents[0] = ias[p1];
		oponents[0]->setInput(game.getPlayerView(Othello::PNoir));
		for (int p2 = p1 + 1; p2 < NB_IA_PER_POOL; ++p2) {
			oponents[1] = ias[p2];
			oponents[1]->setInput(game.getPlayerView(Othello::PBlanc));
			game.reset();
			
			while (!game.isFinish()) {
				if (game.canPlay(current)) {
					int coup = oponents[current - 1]->getOutput(game.getPlayableSpot(current));
					game.play(current, coup / 8, coup % 8);
				}
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
	for (int p = 0; p < NB_IA_PER_POOL; ++p)
		if (score[p] > score[winner])
			winner = p;
	return ia[winner];
}

NeuronalNetwork* tournament() {
	NeuronalNetwork* pool_winners[NB_IA_PER_POOL]{};
	for (int pool = 0; pool < 5; ++pool) {
		pool_winners[pool] = runPool(ias + NB_IA_PER_POOL * pool);
	}
	return runPool(pool_winners);
}

NeuronalNetwork* generation() {
	NeuronalNetwork* winner = tournament();
	for (int ia = 0; ia < 25; ++ia) {
		if (ias[ia] != winner)
			ias[ia]->reproduce(*winner);
	}
	return winner;
}

// ---------------- //


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
			return "Joueur : " + std::to_string(game.getScore(Othello::PNoir)) + " --- IA : " + std::to_string(game.getScore(Othello::PBlanc));
		}
	else
		switch (game.winner())
		{
		case Othello::PNoir:
			return ((gameState.gameMode == 0x00) ? "Victoire des Noirs (" : "Victoire (") + std::to_string(game.getScore(Othello::PNoir)) + " contre " + std::to_string(game.getScore(Othello::PBlanc)) + ") !";
		case Othello::PBlanc:
			return ((gameState.gameMode == 0x00) ? "Victoire des Blancs (" : "Défaîte (") + std::to_string(game.getScore(Othello::PBlanc)) + " contre " + std::to_string(game.getScore(Othello::PNoir)) + ") !";
		case Othello::Nobody:
			return "Égalité !";
		}
	return "Unknown gamemode !";
}



int main() {
	for (int i = 0; i < NB_IA; ++i) {
		ias[i] = new NeuronalNetwork();
		ias[i]->generate();
	}


	NeuronalNetwork* iaOpon = ias[0];
	std::cout << "Le programme genere au prealable une IA a reseau de neurones artificiels.\nVeuillez indiquez le nombre de generations d'IA que vous souhaitez entrainer : ";
	if (!(std::cin >> gameState.nbGenIA))
		return -1;
	const int update_prC = (int)(gameState.nbGenIA * .001) + 1;

	std::cout << "\nProgression : 0 %";
	for (int g = 0; g < gameState.nbGenIA; ++g) {
		iaOpon = generation();
		// Barre de progrès
		if (g % update_prC == 0)
			std::cout << "\rProgression : " << g * 1000 / gameState.nbGenIA * .1 << "%  ";
	}
	std::cout << "\rProgression : 100%  ";

	// --- Graphics --- //

	texPion.loadFromFile("ressource\\pion.png");
	gFont.loadFromFile("ressource\\arial.ttf");

	sf::RenderWindow wndw{ sf::VideoMode(HOW_BIG * 10,HOW_BIG * 10),"Othello" };
	wndw.setFramerateLimit(50);

	Othello Ghandler{};
	iaOpon->setInput(Ghandler.getPlayerView(Othello::PBlanc));
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
	bool playerControl = true; // Les joueurs peuvent-ils intéragir avec le plateau ? (Faux <=> IA Cooldown <=> [Multijoueur local])
	int iaAnswerCooldown = -1; // -1 : c'est au joueur de jouer
	int playX = 0, playY = 0; // Les coordonnées du coup qui va être joué
	Othello::Player current = Othello::PNoir;


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
					playX = (event.mouseButton.y - HOW_BIG) / HOW_BIG;
					playY = (event.mouseButton.x - HOW_BIG) / HOW_BIG;
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::R: // Relance la partie depuis 0
					Ghandler.reset();
					Grender.update();
					isStarted = false;
					playerControl = true;
					iaAnswerCooldown = -1;
					current = Othello::PNoir;
					PIndic.setTextureRect({ 20 * (current - 1), 0, 20 * current, 20 });
					info.setString(getGameInfo(Ghandler));
					break;
				case sf::Keyboard::M: // Change de mode de jeu (multi ou ia)
					if (!isStarted) {
						gameState.gameMode = (gameState.gameMode + 1) % 2;
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
				int play = iaOpon->getOutput(Ghandler.getPlayableSpot(current));
				playX = play / 8;
				playY = play % 8;
			}
			if (iaAnswerCooldown >= 0)
				--iaAnswerCooldown;
		}
		// ----------------------- //

		// --- Gestion des tours de jeux --- //
		if (playChoosen) { // Si le choix du coup a été effectué
			playChoosen = false;
			if (renderPlay(Ghandler, Grender, current, playX, playY)) { // Si le coup est valide
				info.setString(getGameInfo(Ghandler));
				if (Ghandler.canPlay((Othello::Player)(current % 2 + 1))) {
					current = (Othello::Player)(current % 2 + 1);
					if (gameState.gameMode == 0x01) {
						playerControl = !playerControl;
					}
					PIndic.setTextureRect({ 20 * (current - 1), 0, 20, 20 });
				}
				if (!playerControl && gameState.gameMode == 0x01)
					iaAnswerCooldown = gameState.iaAnswerDelay;
			}
		}
		// --------------------------------- //

		wndw.clear(sf::Color(200,200,200));
		wndw.draw(Grender);
		wndw.draw(PIndic);
		wndw.draw(info);
		wndw.display();
	}

}