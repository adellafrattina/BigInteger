#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <limits>

#undef BI_PRINT_DEBUG_INFO
//#define BI_PRINT_DEBUG_INFO

#ifndef BI_STATIC
	#if defined(_WIN32)
		#ifdef BI_BUILD_DLL
			#define BI_API __declspec(dllexport)
		#else
			#define BI_API __declspec(dllimport)
		#endif
		#ifdef _MSC_VER
			#pragma warning(disable : 4251)
		#endif
	#elif defined(__APPLE__)
		#define BI_API __attribute__((visibility("default")))
	#else
		#error Dynamic linking is not available on the current OS
	#endif
#else
	#define BI_API
#endif

// For debug purposes
#undef PRINT
#if !defined(BI_PRINT_DEBUG_INFO)
	#define PRINT(fmt, ...)
#else

	#include <cstdio>
	#include <cstdarg>

	static void g_biprintfln(const char* fmt, const char* time, const char* filename, int line, ...) {

		va_list args;
		va_start(args, line);
		printf("[%s][%s:%d] ", time, filename, line);
		vprintf(fmt, args);
		va_end(args);
		putchar('\n');
	}

	// To print debug information on standard output
	#define PRINT(fmt, ...) g_biprintfln(fmt, __TIME__, (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)), __LINE__, __VA_ARGS__)
#endif

// The operating system word
typedef std::uint64_t WORD;

#define BI_PLUS_SIGN 0
#define BI_MINUS_SIGN std::numeric_limits<bool>::max()

// The big integer base structure type
class BI_API BigInt_T {

public:

	BigInt_T();
	BigInt_T(WORD sno, bool sign);
	BigInt_T(WORD* buffer, std::size_t size, bool sign);
	BigInt_T(const BigInt_T& other);
	BigInt_T(BigInt_T&& other) noexcept;
	BigInt_T& operator=(const BigInt_T& other);
	BigInt_T& operator=(BigInt_T&& other) noexcept;
	~BigInt_T();

	// The big integer buffer
	WORD* Buffer;

	// The buffer size
	std::size_t Size;

	// The big integer sign
	bool Sign;

	// Small Number Optimization
	WORD SNO;
};

namespace big {

	class BI_API Integer {

	public:

		static void Rand(big::Integer& n);

		template <
			typename T,
			typename = typename std::enable_if<std::is_integral<T>::value>::type
		>
		Integer(T n, std::size_t capacity = 0) {

			if ((std::int64_t)n < 0)
				InitFromInt(-(std::int64_t)n, BI_MINUS_SIGN, capacity);
			else
				InitFromInt(n, BI_PLUS_SIGN, capacity);
		}

		Integer(const std::string& str, std::size_t capacity = 0);
		Integer(const char* str, std::size_t capacity = 0);
		Integer(const Integer& other, std::size_t capacity = 0);
		Integer(std::nullptr_t, std::size_t capacity = 0) = delete;
		Integer& operator=(const Integer& other);
		Integer(Integer&& other) noexcept;
		Integer();
		~Integer();

		static bool FromString(Integer& data, const std::string& str);

		std::string ToString() const;
		const void* Data();
		std::size_t Size() const;
		void Resize(std::size_t size);
		void ShrinkToFit();
		void Clear();

		// Stream

		friend BI_API std::istream& operator>>(std::istream& is, big::Integer& n);
		friend BI_API std::ostream& operator<<(std::ostream& os, const big::Integer& n);

		// Operator overloading

		friend BI_API const big::Integer& operator++(big::Integer& n);
		friend BI_API big::Integer operator++(big::Integer& n, int);

		friend BI_API const big::Integer& operator--(big::Integer& n);
		friend BI_API big::Integer operator--(big::Integer& n, int);

		friend BI_API const big::Integer& operator+(big::Integer& n);
		friend BI_API const big::Integer operator+(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer& operator+=(big::Integer& a, const big::Integer& b);

		friend BI_API const big::Integer& operator-(big::Integer& n);
		friend BI_API const big::Integer operator-(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer& operator-=(big::Integer& a, const big::Integer& b);

		friend BI_API const big::Integer operator*(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer& operator*=(big::Integer& a, const big::Integer& b);

	private:

		void InitFromInt(WORD n, bool sign, std::size_t capacity);

		// Normal big integer data structure
		BigInt_T m_Data;
	};
}
