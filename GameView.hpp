#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include<SFML/Graphics.hpp>

struct GameAppear {
	sf::Vector2i plateau_size;
	int nb_player;
	const sf::Texture* tex_pion;
	sf::Vector2i tex_size;
};

class GameView : public sf::Drawable, public sf::Transformable {
public:
	GameView(const GameAppear& gapp, const char* board, int case_size = 30);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update();
	void update(int x, int y);
private:
	void InitCasesLineS();
private:
	sf::VertexArray m_cases, m_lines;
	GameAppear m_gapp;
	const char* m_board;
	int m_case_size;
};

#endif // !GAME_VIEW_HPP
