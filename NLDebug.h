#ifndef NL_DEBUG_HPP
#define NL_DEBUG_HPP


// Macro pour activer un mode de débuggage poussé
#define NL_DEBUG

enum DebugInterrupt {
	IndexOutOfRange,
	BadSizeArgument
};

void debug_break(enum DebugInterrupt di, const char* message);
void alloc_test(void* pointer, const char* str);

#endif