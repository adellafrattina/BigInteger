#pragma once

#if defined(BI_DEBUG)
	#ifdef BI_PLATFORM_WINDOWS
		#include <Windows.h>
		#define BI_DEBUGBREAK() DebugBreak()
	#elif defined(BI_PLATFORM_LINUX)
		#include <signal.h>
		#define BI_DEBUGBREAK() raise(SIGTRAP)
	#elif defined(BI_PLATFORM_MACOSX)
		#include <TargetConditionals.h>
		#define BI_DEBUGBREAK() abort()
	#else
		#error "Platform does not support debugbreak yet!"
	#endif
	#define BI_ENABLE_ASSERTS
#else
	#define BI_DEBUGBREAK()
#endif

#define BIT(x) (1 << x)

#include "Log.hpp"

#ifdef BI_ENABLE_ASSERTS
	#define BI_ASSERT(x, ...)          if(!(x)) { BI_ERROR     ("Assertion Failed: {0}", __VA_ARGS__); BI_DEBUGBREAK(); }
#else
	#define BI_ASSERT(x, ...) 0
#endif
