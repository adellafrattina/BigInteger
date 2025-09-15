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

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(__LP64__)

	// The operating system word
	typedef std::uint64_t WORD;

	// The operating system half word
	typedef std::uint32_t HALFWORD;

#elif defined(__i386__) || defined(_M_IX86) || defined(__ILP32__) || defined(__i686__)

	// The operating system word
	typedef std::uint32_t WORD;

	// The operating system half word
	typedef std::uint16_t HALFWORD;

#elif defined(_WIN64)

	// The operating system word
	typedef std::uint64_t WORD;

	// The operating system half word
	typedef std::uint32_t HALFWORD;

#elif defined(_WIN32)

	// The operating system word
	typedef std::uint32_t WORD;

	// The operating system half word
	typedef std::uint16_t HALFWORD;

#else
	#error Unknown system architecture
#endif

#define BI_PLUS_SIGN 0
#define BI_MINUS_SIGN std::numeric_limits<bool>::max()
#define BI_MAX_WORD std::numeric_limits<WORD>::max()
#define BI_MAX_HALFWORD std::numeric_limits<HALFWORD>::max()

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

	/// <summary>
	/// The class that represents an integer without overflow or underflow
	/// </summary>
	class BI_API Integer {

	public:

		/// <summary>
		/// Set a new seed for the random function
		/// </summary>
		/// <param name="seed">The desired seed</param>
		static void Seed(unsigned int seed);

		/// <summary>
		/// Generate a random number
		/// </summary>
		/// <param name="n">The desired number (must already have a size)</param>
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

		/// <summary>
		/// Tries to convert a string into a big integer
		/// </summary>
		/// <param name="data">The converted big integer</param>
		/// <param name="str">The string to convert that represents a number</param>
		/// <returns>True if the conversion succeeds, false if there is at least one character that is not a digit (exception made for the minus sign at the beginning)</returns>
		static bool FromString(Integer& data, const std::string& str);

		/// <summary>
		/// Converts a big integer into a string
		/// </summary>
		/// <returns>The big integer as a string</returns>
		std::string ToString() const;

		/// <returns>The raw big integer allocator</returns>
		const void* Data();

		/// <returns>The limb size of the big integer allocator (a limb is an OS word)</returns>
		std::size_t Size() const;

		/// <summary>
		/// Resizes the big integer allocator. It will keep the original content (if the new size is greater or equal to the old one)
		/// </summary>
		/// <param name="size">The desired size as limbs (a limb is an OS word)</param>
		void Resize(std::size_t size);

		/// <summary>
		/// Removes unnecessary limbs that are irrelevant for the big integer representation (a limb is an OS word)
		/// </summary>
		void ShrinkToFit();

		/// <summary>
		/// Deletes the big integer allocator, setting it to the default value (an OS word equals to zero)
		/// </summary>
		void Clear();

		// Stream

		friend BI_API std::istream& operator>>(std::istream& is, big::Integer& n);
		friend BI_API std::ostream& operator<<(std::ostream& os, const big::Integer& n);

		// Arithmetic operators

		friend BI_API const big::Integer operator+(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator-(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator*(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator/(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator%(const big::Integer& a, const big::Integer& b);

		// Relational and comparison operators

		friend BI_API const bool operator==(const big::Integer& a, const big::Integer& b);
		friend BI_API const bool operator!=(const big::Integer& a, const big::Integer& b);
		friend BI_API const bool operator<(const big::Integer& a, const big::Integer& b);
		friend BI_API const bool operator>(const big::Integer& a, const big::Integer& b);
		friend BI_API const bool operator<=(const big::Integer& a, const big::Integer& b);
		friend BI_API const bool operator>=(const big::Integer& a, const big::Integer& b);

		// Logical operators

		explicit operator bool() const;

		// Bitwise operators

		friend BI_API const big::Integer operator&(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator|(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator^(const big::Integer& a, const big::Integer& b);
		friend BI_API const big::Integer operator~(const big::Integer& n);
		friend BI_API const big::Integer operator<<(const big::Integer& n, std::size_t bits);
		friend BI_API const big::Integer operator>>(const big::Integer& n, std::size_t bits);
		friend BI_API const big::Integer operator<<(const big::Integer& n, big::Integer bits);
		friend BI_API const big::Integer operator>>(const big::Integer& n, big::Integer bits);

		// Assignment operators

		friend BI_API big::Integer& operator+=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator-=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator*=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator/=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator%=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator&=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator|=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator^=(big::Integer& a, const big::Integer& b);
		friend BI_API big::Integer& operator<<=(big::Integer& n, std::size_t bits);
		friend BI_API big::Integer& operator>>=(big::Integer& n, std::size_t bits);
		friend BI_API big::Integer& operator<<=(big::Integer& n, big::Integer bits);
		friend BI_API big::Integer& operator>>=(big::Integer& n, big::Integer bits);

		// Unary operators

		friend BI_API big::Integer& operator+(big::Integer& n);
		friend BI_API big::Integer& operator++(big::Integer& n);
		friend BI_API big::Integer operator++(big::Integer& n, int);
		friend BI_API big::Integer& operator-(big::Integer& n);
		friend BI_API big::Integer& operator--(big::Integer& n);
		friend BI_API big::Integer operator--(big::Integer& n, int);

	private:

		void InitFromInt(WORD n, bool sign, std::size_t capacity);

		// Big integer data structure
		BigInt_T m_Data;
	};
}
