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
	// To print debug information on standard output
	#define PRINT(fmt, ...) printf(fmt, __VA_ARGS__); putchar('\n');
#endif

typedef std::uint8_t bi_type;

struct bi_int {

	bi_type* Buffer = nullptr;
	std::size_t Size = 0;
};

#define BI_MAX_INT std::numeric_limits<bi_type>::max()
#define BI_PLUS_SIGN 0
#define BI_MINUS_SIGN BI_MAX_INT

namespace bi {

	class BI_API Integer {

	public:

		Integer(std::int64_t n);
		Integer(const std::string& str);
		Integer(const Integer& other);
		Integer(Integer&& other) noexcept;
		Integer();
		~Integer();

		std::string ToString() const;
		const void* Data();
		std::size_t Size() const;
		std::size_t SizeInBytes() const;

		Integer& operator=(std::int64_t n);
		Integer& operator=(const std::string& str);
		Integer& operator=(const Integer& other);

		// Plus

		Integer& operator+();

		Integer operator+(std::int64_t n) const;
		Integer& operator+=(std::int64_t n);

		Integer operator+(const std::string& str) const;
		Integer& operator+=(const std::string& str);

		Integer operator+(const Integer& other) const;
		Integer& operator+=(const Integer& other);

		Integer operator++(int);
		Integer& operator++();

		// Minus

		Integer operator-();

		Integer operator-(std::int64_t n) const;
		Integer& operator-=(std::int64_t n);

		Integer operator-(const std::string& str) const;
		Integer& operator-=(const std::string& str);

		Integer operator-(const Integer& other) const;
		Integer& operator-=(const Integer& other);

		Integer operator--(int);
		Integer& operator--();

		// Star

		Integer operator*(std::int64_t n) const;
		Integer& operator*=(std::int64_t n);

		Integer operator*(const std::string& str) const;
		Integer& operator*=(const std::string& str);

		Integer operator*(const Integer& other) const;
		Integer& operator*=(const Integer& other);

		// Backslash

		Integer operator/(const Integer& other) const;

		// Boolean

		operator bool() const;

		bool operator==(const std::string& str) const;
		bool operator==(const Integer& other) const;

		bool operator!=(const std::string& str) const;
		bool operator!=(const Integer& other) const;

		bool operator>(const std::string& str) const;
		bool operator>(const Integer& other) const;

		bool operator<(const std::string& str) const;
		bool operator<(const Integer& other) const;

		bool operator>=(const std::string& str) const;
		bool operator>=(const Integer& other) const;

		bool operator<=(const std::string& str) const;
		bool operator<=(const Integer& other) const;

		// Stream

		friend BI_API std::istream& operator>>(std::istream& is, bi::Integer& n);
		friend BI_API std::ostream& operator<<(std::ostream& os, const bi::Integer& n);

	private:

		void Init(std::int64_t n);
		bool Init(const std::string& str);

		mutable bi_int m_Data; // Mutable key is temporary
	};
}
