#ifndef WIN_H
#define WIN_H

#include <include/util/compile.h>

#if defined(_WIN64) || defined(_WIN32) || defined(_WIN32_WCE)
DISPLAYMESSAGE("Windows platform detected", WIN64)
#elif defined(__CYGWIN64__)
DISPLAYMESSAGE("Cygwin platform detected", CYGWIN64)
#elif defined(__CYGWIN__)
DISPLAYWARNING("Cygwin platform detected, compatibility not guaranteed")
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
DISPLAYMESSAGE("Windows platform detected, MSVC compiler detected")
#else
DISPLAYWARNING("Windows platform detected, non-MSVC compiler detected, compatibility not guaranteed")
#if defined(__GNUC__) || defined(__GNUG__)
DISPLAYWARNING("GCC/MinGW compiler detected, compatibility not guaranteed")
#define CUSTOM_COMPILER = 0x1
#elif defined(__clang__)
DISPLAYWARNING("Clang compiler detected, compatibility not guaranteed")
#define CUSTOM_COMPILER = 0x2
#else
DISPLAYERROR("Unknown compiler, compatibility not guaranteed")
#define CUSTOM_COMPILER = 0x0

#endif
#endif
#endif
#endif

#if defined(_WIN32_WINNT ) && (_WIN32_WINNT == 0x0A00)
DISPLAYMESSAGE("Windows 10 or later detected")
#else
DISPLAYWARNING("Windows 10 or later not detected, compatibility not guaranteed")
#endif

#if __has_include(<filesystem>)
#include <filesystem>
#endif