#include <stdio.h>
#include <stdarg.h>

#include "Log.hpp"

#define PRINT(fmt) \
		va_list argptr; \
		va_start(argptr, fmt); \
		__crt_va_start(argptr, fmt); \
		_vfprintf_l(stdout, fmt, NULL, argptr); \
		__crt_va_end(argptr) \

namespace bi {

	void Log::Trace(const char* fmt, ...) {

		printf("[TRACE] ");
		PRINT(fmt);
	}

	void Log::Info(const char* fmt, ...) {

		printf("[TRACE] ");
		PRINT(fmt);
	}

	void Log::Warn(const char* fmt, ...) {

		printf("[TRACE] ");
		PRINT(fmt);
	}

	void Log::Error(const char* fmt, ...) {

		printf("[TRACE] ");
		PRINT(fmt);
	}

	void Log::Critical(const char* fmt, ...) {

		printf("[TRACE] ");
		PRINT(fmt);
	}
}
