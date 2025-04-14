#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <limits>

#undef BI_PRINT_DEBUG_INFO
#define BI_PRINT_DEBUG_INFO

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

// For debug purposes
#undef PRINT
#if !defined(BI_PRINT_DEBUG_INFO)
	#define PRINT(fmt, ...)
#else
	// To print debug information on standard output
	#define PRINT(fmt, ...) printf(fmt, __VA_ARGS__); putchar('\n');
#endif

typedef std::uint8_t bi_int;
#define BI_MAX_INT std::numeric_limits<bi_int>::max()

namespace bi {

	class BI_API Integer {

	public:

		Integer(std::uint64_t n);
		Integer(const std::string& str);
		Integer(const Integer& other);
		Integer(Integer&& other) noexcept;
		Integer();
		~Integer();

		std::string ToString() const;
		const void* Data();
		std::size_t Size() const;
		std::size_t SizeInBytes() const;

		Integer& operator=(std::uint64_t n);
		Integer& operator=(const std::string& str);
		Integer& operator=(const Integer& other);

		// Plus

		Integer& operator+();

		Integer operator+(std::uint64_t n);
		Integer& operator+=(std::uint64_t n);

		Integer operator+(const std::string& str);
		Integer& operator+=(const std::string& str);

		Integer operator+(const Integer& other);
		Integer& operator+=(const Integer& other);

		Integer operator++(int);
		Integer& operator++();

		// Minus

		Integer operator-();

		Integer operator-(std::uint64_t n);
		Integer& operator-=(std::uint64_t n);

		Integer operator-(const std::string& str);
		Integer& operator-=(const std::string& str);

		Integer operator-(const Integer& other);
		Integer& operator-=(const Integer& other);

		Integer operator--(int);
		Integer& operator--();

		// Star

		Integer operator*(const Integer& other);

		// Backslash

		Integer operator/(const Integer& other);

		// Boolean

		bool operator==(const Integer& other);
		bool operator!=(const Integer& other);
		bool operator>(const Integer& other);
		bool operator<(const Integer& other);
		bool operator>=(const Integer& other);
		bool operator<=(const Integer& other);

		// Stream

		friend BI_API std::istream& operator>>(std::istream& is, bi::Integer& n);
		friend BI_API std::ostream& operator<<(std::ostream& os, const bi::Integer& n);

	private:

		bool Init(const std::string& str);
		void Init(const std::uint64_t& n);
		void Clear();

		mutable bi_int* m_Data;
		mutable std::size_t m_Size;
	};
}
