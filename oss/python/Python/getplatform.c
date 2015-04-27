
#include "Python.h"

#ifndef PLATFORM
#ifdef __CYGWIN__
#define PLATFORM "cygwin"
#elif defined(_WIN32)
#define PLATFORM "win32"
#elif defined(__APPLE__) && defined(__MACH__)
#define PLATFORM "darwin"
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM "linux2"
#elif defined(__EMX__)
#define PLATFORM "os2emx"
#elif defined(OS2) || defined(_OS2) || defined(__OS2__)
#define PLATFORM "os2"
#else
#define PLATFORM "unknown"
#endif
#endif

const char *
Py_GetPlatform(void)
{
	return PLATFORM;
}
