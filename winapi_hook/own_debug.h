#include <string.h>
#include <stdio.h>
#include <Windows.h>

#ifndef OWN_DEBUG
#define OWN_DEBUG

#ifdef DEBUG_MODE

#define PDEBUG(message, type)						\
	printf("[%s] - %s\n", (type), (message));		\
	if (!strcmp((type), "CRITICAL")) DebugBreak();

#else

#define PDEBUG(message, type)

#endif // DEBUG_MODE

#endif // OWN_DEBUG