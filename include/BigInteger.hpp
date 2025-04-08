#pragma once

#include <istream>
#include <ostream>
#include <string>

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

		Integer(const std::string& str);
		Integer(std::uint64_t n);
		Integer(const Integer& other);
		Integer(Integer&& other) noexcept;
		Integer();
		~Integer();

		std::string ToString() const;
		const void* Data();
		std::size_t Size() const;

		Integer& operator=(const std::string& str);
		Integer& operator=(std::uint64_t n);
		Integer& operator=(const Integer& other);

		Integer operator+(const Integer& other);
		Integer operator++();
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

		Integer(std::uint32_t* data, std::size_t size);

		bool Init(const std::string& str);
		void Init(const std::uint64_t& n);
		void Clear();

		std::uint32_t* m_Data;
		std::size_t m_Size;
	};
}
