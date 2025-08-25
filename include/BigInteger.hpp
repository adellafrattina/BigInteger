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

// The big integer buffer's type
typedef std::uint8_t bi_type;

// The operating system word
typedef std::uint64_t WORD;

// The big integer structure
class BI_API bi_int {

public:

	bi_int();
	bi_int(WORD sno);
	bi_int(bi_type* buffer, std::size_t size);
	bi_int(const bi_int& other);
	bi_int(bi_int&& other) noexcept;
	bi_int& operator=(const bi_int& other);
	bi_int& operator=(bi_int&& other) noexcept;
	~bi_int();

	// The big integer buffer
	bi_type* Buffer;

	// The buffer size
	std::size_t Size;

	/// <summary>
	/// Gets the Small Number Optimization value in an endianness friendly way.
	/// The value zero could also mean that the big integer value was allocated on the heap and not on the stack, so the SNO is invalid
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>The Small Number Optimization value</returns>
	friend WORD GetSNO(const bi_int& data);

	/// <summary>
	/// Sets the Small Number Optimization value in an endianness friendly way.
	/// The provided big integer should not have its buffer allocated on the heap.
	/// To be sure, check that the big integer size is the size of a WORD over the size of bi_type
	/// </summary>
	/// <param name="data">The desired data</param>
	/// <param name="sno">The desired value for the SNO</param>
	friend void SetSNO(bi_int& data, WORD sno);

	/// <summary>
	/// Resets the big integer to the default value
	/// </summary>
	friend void Reset(bi_int& data);

private:

	// Small Number Optimization
	WORD m_SNO = 0;
};

#define BI_MAX_INT std::numeric_limits<bi_type>::max()
#define BI_PLUS_SIGN 0
#define BI_MINUS_SIGN BI_MAX_INT

namespace big {

	class BI_API Integer {

	public:

		template <
			typename T,
			typename = typename std::enable_if<std::is_integral<T>::value>::type
		>
		Integer(T n, std::size_t size_in_bytes = 0) {

			InitFromInt(n, size_in_bytes);
		}

		Integer(const std::string& str, std::size_t size_in_bytes = 0);
		Integer(const char* str, std::size_t size_in_bytes = 0);
		Integer(const Integer& other, std::size_t size_in_bytes = 0);
		Integer(std::nullptr_t, std::size_t size_in_bytes = 0) = delete;
		Integer& operator=(const Integer& other);
		Integer(Integer&& other) noexcept;
		Integer();
		~Integer();

		static bool FromString(Integer& data, const std::string& str);

		std::string ToString() const;
		const void* Data();
		std::size_t Size() const;
		std::size_t SizeInBytes() const;
		void Resize(std::size_t size_in_bytes, bool ext_sign = true);
		void ShrinkToFit();
		void Clear();

		// Stream

		friend BI_API std::istream& operator>>(std::istream& is, big::Integer& n);
		friend BI_API std::ostream& operator<<(std::ostream& os, const big::Integer& n);

		// Operator overloading

		friend BI_API big::Integer operator+(const big::Integer& first, const big::Integer& second);
		friend BI_API big::Integer& operator+=(big::Integer& first, const big::Integer& second);

		friend BI_API big::Integer operator*(const big::Integer& first, const big::Integer& second);
		friend BI_API big::Integer& operator*=(big::Integer& first, const big::Integer& second);

	private:

		void InitFromInt(std::int64_t n, std::size_t size_in_bytes);

		// Normal big integer data structure
		bi_int m_Data;
	};
}
