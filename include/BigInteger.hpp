#pragma once

#include <istream>
#include <ostream>

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

namespace bi {

	class BI_API Integer {

	public:

		Integer(long long n);
		Integer(const Integer& other);
		Integer(Integer&& other) noexcept;
		Integer();

		Integer& operator=(const Integer& other);

		Integer operator+(const Integer& other);
		Integer operator-(const Integer& other);
		Integer operator*(const Integer& other);
		Integer operator/(const Integer& other);

		bool operator==(const Integer& other);
		bool operator!=(const Integer& other);
		bool operator>(const Integer& other);
		bool operator<(const Integer& other);
		bool operator>=(const Integer& other);
		bool operator<=(const Integer& other);

		friend BI_API std::istream& operator>>(std::istream& is, bi::Integer& n);
		friend BI_API std::ostream& operator<<(std::ostream& os, const bi::Integer& n);

	private:

		long long m_Data;
	};
}
