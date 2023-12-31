#include "NLDebug.h"

#include<io.h>

void debug_break(enum DebugInterrupt di, const char* message) {
	perror(message);
	switch (di)
	{
	case IndexOutOfRange:
		break;
	case BadSizeArgument:
		break;
	default:
		break;
	}
	exit(-1);
}

void alloc_test(void* pointer, const char* str)
{
	if (pointer == NULL) {
		perror(str);
		exit(-5);
	}
}
