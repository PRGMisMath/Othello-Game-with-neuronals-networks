#include "console.hpp"


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

bool errorno(const char* message, bool condition)
{
	if (condition)
		return false;
	std::clog << "Erreur : " << message;

	return true;
}

void console(args a) {
	int nb_gen = 0;


	std::string entree;
	std::vector<std::string> cmd;
	while (true) {
		std::cout << "> ";
		std::getline(std::cin, entree);
		chunk(entree);
		if (*a.isAIPlaying)
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
			*a.isAIGenerating = true;
			if (cmd.size() == 1) {
				std::cout << "Generation : " << ++nb_gen << "\n";
				*a.opon = generation(a.ias);
			}
			else if (cmd.size() == 2) try {
				int boucle = std::stoi(cmd[1]);
				for (int i = 0; i < boucle; ++i) {
					std::cout << "Generation : " << ++nb_gen << "\n";
					*a.opon = generation(a.ias);
				}
			} catch (const std::exception& e) {
				errorno("Argument invalide !");
				std::cout << std::endl;
				continue;
			}
			else 
				errorno("Trop d'arguments !");
			*a.isAIGenerating = false;
		}
		else 
			errorno("Commande non reconnue !");
		std::cout << std::endl;
	}
}