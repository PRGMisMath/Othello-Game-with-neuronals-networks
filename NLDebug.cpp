#include "NLDebug.hpp"

void nl::debug_break(DebugInterrupt di, const char* message) {
	std::cerr << message << std::endl;
	switch (di)
	{
	case nl::IndexOutOfRange:
		break;
	case nl::BadSizeArgument:
		break;
	default:
		break;
	}
	throw std::exception("DEBUG BREAK");
}
