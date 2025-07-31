#include <cassert>
#include <cmath>

#include "Utils.hpp"

#include "BigInteger.hpp"

namespace big {

	Integer::Integer(WORD n, std::size_t size_in_bytes)
		: m_Data()

	{

		if (size_in_bytes > sizeof(WORD)) {

			Utils::Resize(m_Data, size_in_bytes, false);
			bi_int tmp = Utils::BigIntegerFromWORD(n);
			Utils::Copy(m_Data, tmp);
		}

		else {

			m_Data = Utils::BigIntegerFromWORD(n);
		}
	}

	Integer::Integer(const std::string& str, std::size_t size_in_bytes)
		: m_Data()

	{

		// @TODO
	}

	Integer::Integer(const Integer& other, std::size_t size_in_bytes)
		: m_Data()

	{

		if (size_in_bytes > sizeof(WORD)) {

			Utils::Resize(m_Data, size_in_bytes + other.m_Data.Size, false);
			bi_int tmp = Utils::BigIntegerFromWORD(GetSNO(other.m_Data));
			Utils::Copy(m_Data, tmp);
		}

		else {

			if (Utils::IsOnStack(other.m_Data))
				m_Data = Utils::BigIntegerFromWORD(GetSNO(other.m_Data));

			else {

				Utils::Resize(m_Data, other.m_Data.Size, false);
				Utils::Copy(m_Data, other.m_Data);
			}
		}
	}

	Integer::Integer()
		: m_Data()

	{}

	Integer::~Integer() {

		Utils::Clear(m_Data);
	}

	Integer Integer::FromString(const std::string& n) {

		return Integer();
	}

	std::string Integer::ToString() const {

		return "";
	}

	const void* Integer::Data() {

		return m_Data.Buffer;
	}

	std::size_t Integer::Size() const {

		return m_Data.Size;
	}

	std::size_t Integer::SizeInBytes() const {

		return m_Data.Size * sizeof(bi_type);
	}

	void Integer::Resize(std::size_t size_in_bytes) {

		Utils::Resize(m_Data, size_in_bytes);
	}

	// Stream

	std::istream& operator>>(std::istream& is, big::Integer& n) {

		std::string str;
		is >> str;
		bi_int tmp = Integer::FromString(str).m_Data;
		if (tmp.Buffer == nullptr)
			is.setstate(std::ios_base::failbit);
		else
			n.m_Data = tmp;

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const big::Integer& n) {

		return os << n.ToString();
	}
}

bi_int::bi_int()
	: Buffer(nullptr), Size(sizeof(WORD) / sizeof(bi_type)), m_SNO(0)

{

	Buffer = reinterpret_cast<bi_type*>(&m_SNO);
}

bi_int::bi_int(WORD sno)
	: Buffer(nullptr), Size(sizeof(WORD) / sizeof(bi_type)), m_SNO(sno)

{

	Buffer = reinterpret_cast<bi_type*>(&m_SNO);
}

bi_int::bi_int(bi_type* buffer, std::size_t size)
	: Buffer(buffer), Size(size), m_SNO(0)

{}

bi_int::bi_int(const bi_int& other)
	: Buffer(nullptr), Size(other.Size), m_SNO(other.m_SNO)

{

	if (Size == sizeof(WORD) / sizeof(bi_type))
		Buffer = reinterpret_cast<bi_type*>(&m_SNO);
	else
		Buffer = other.Buffer;
}

bi_int& bi_int::operator=(const bi_int& other) {

	Size = other.Size;
	m_SNO = other.m_SNO;
	if (Size == sizeof(WORD) / sizeof(bi_type))
		Buffer = reinterpret_cast<bi_type*>(&m_SNO);
	else
		Buffer = other.Buffer;

	return *this;
}
