#pragma once

#include "BigInteger/Base.hpp"

namespace bi {

	class BI_API Log {

	public:

		static void Trace(const char* fmt, ...);
		static void Info(const char* fmt, ...);
		static void Warn(const char* fmt, ...);
		static void Error(const char* fmt, ...);
		static void Critical(const char* fmt, ...);
	};
}

#if defined(BI_RELEASE)
#define BI_TRACE(fmt, ...)
#define BI_INFO(fmt, ...)
#define BI_WARN(fmt, ...)
#define BI_ERROR(fmt, ...)
#define BI_CRITICAL(fmt, ...)
#else
#define BI_TRACE(fmt, ...)         bi::Log::Trace(fmt, __VA_ARGS__)
#define BI_INFO(fmt, ...)          bi::Log::Info(fmt, __VA_ARGS__)
#define BI_WARN(fmt, ...)          bi::Log::Warn(fmt, __VA_ARGS__)
#define BI_ERROR(fmt, ...)         bi::Log::Error(fmt, __VA_ARGS__)
#define BI_CRITICAL(fmt, ...)      bi::Log::Critical(fmt, __VA_ARGS__)
#endif
