#pragma once

#define LD31_VERSION_MAJOR 0
#define LD31_VERSION_MINOR 0
#define LD31_VERSION_PATCH 0
#define LD31_VERSION_TWEAK 1

#define LD31_VERSION "0.0.0.1"

#if false
#  define LD31_GIT_DESCRIBE "-128-NOTFOUND"
#else
#  define LD31_GIT_DESCRIBE "Out-of-Git build"
#endif

#if ((defined _WIN32) || (defined _WIN64)) && (!defined __CYGWIN__)
#  define LD31_WINDOWS
#  define WIN31_LEAN_AND_MEAN
#elif ((defined __APPLE__) && (defined __MACH__))
#  define LD31_MACOSX
#  define LD31_POSIX
#elif (defined __linux)
#  define LD31_LINUX
#  define LD31_UNIX
#  define LD31_POSIX
#elif (defined unix) || (defined __unix__)
#  define LD31_UNIX
#  define LD31_POSIX
#endif

#if (defined __x86_64__) || (defined _M_X64) || (defined __ia64___)
#  define LD31_64
#else
#  define LD31_32
#endif

#if (defined _DEBUG) && (!defined NDEBUG)
#  define LD31_DEBUG
#else
#  define LD31_RELEASE
#endif
