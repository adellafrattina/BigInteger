#pragma once

#ifndef BI_STATIC
	#ifdef _WIN32
		#ifdef BI_BUILD_DLL
			#define BI_API __declspec(dllexport)
		#else
			#define BI_API __declspec(dllimport)
		#endif
		#ifdef _MSC_VER
			#pragma warning(disable : 4251)
		#endif
	#else
		#error Dynamic linking is not available on the current OS
	#endif
#else
	#define BI_API
#endif
