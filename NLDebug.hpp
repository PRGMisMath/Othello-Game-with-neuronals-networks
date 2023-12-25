#ifndef NL_DEBUG_HPP
#define NL_DEBUG_HPP


// Macro pour activer un mode de debuggage pousse
#define NL_DEBUG

#ifdef NL_DEBUG

#include<iostream>

namespace nl {

enum DebugInterrupt {
	IndexOutOfRange,
	BadSizeArgument
};

void debug_break(DebugInterrupt di, const char* message);

}

#endif

#endif