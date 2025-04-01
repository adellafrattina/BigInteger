#include "Assert.hpp"

#include "BigInteger.hpp"

namespace bi {

	Integer::Integer(long long n)
		: m_Data(n)

	{}

	Integer::Integer(const Integer& other)
		: m_Data(other.m_Data)

	{}

	Integer::Integer(Integer&& other) noexcept
		: m_Data(other.m_Data)

	{

		other.m_Data = 0;
	}

	Integer::Integer()
		: m_Data(0)

	{}

	Integer& Integer::operator=(const Integer& other) {

		m_Data = other.m_Data;

		return *this;
	}

	Integer Integer::operator+(const Integer& other) {

		return Integer(m_Data + other.m_Data);
	}

	Integer Integer::operator-(const Integer& other) {

		return Integer(m_Data - other.m_Data);
	}

	Integer Integer::operator*(const Integer& other) {

		return Integer(m_Data * other.m_Data);
	}

	Integer Integer::operator/(const Integer& other) {

		return Integer(m_Data / other.m_Data);
	}

	bool Integer::operator==(const Integer& other) {

		return m_Data == other.m_Data;
	}

	bool Integer::operator!=(const Integer& other) {

		return m_Data != other.m_Data;
	}

	bool Integer::operator>(const Integer& other) {

		return m_Data > other.m_Data;
	}

	bool Integer::operator<(const Integer& other) {

		return m_Data < other.m_Data;
	}

	bool Integer::operator>=(const Integer& other) {

		return m_Data >= other.m_Data;
	}

	bool Integer::operator<=(const Integer& other) {

		return m_Data <= other.m_Data;
	}

	std::istream& operator>>(std::istream& is, bi::Integer& n) {

		is >> n.m_Data;

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const bi::Integer& n) {

		os << n.m_Data;

		return os;
	}
}
