#ifndef P_CONSOLE_HPP
#define P_CONSOLE_HPP

#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>
#include "generation.hpp"

using namespace std::literals;


struct args {
	std::vector<nl::Network*>& ias; refAI* opon; bool* isAIPlaying; bool* isAIGenerating;
};

bool errorno(const char* message, bool condition = false);
void console(args a);



#endif