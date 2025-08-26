#include <cassert>
#include <cmath>

#include "Utils.hpp"

#include "BigInteger.hpp"

namespace big {

	void Integer::InitFromInt(WORD n, bool sign, std::size_t size_in_bits) {

		size_in_bits = (std::size_t)std::ceil((long double)size_in_bits / (sizeof(WORD) * 8));
		if (size_in_bits > 1) {

			Utils::Resize(m_Data, size_in_bits);
			bi_memcpy(m_Data.Buffer, m_Data.Size * sizeof(WORD), &n, sizeof(WORD));
			m_Data.Sign = sign;
		}

		else
			m_Data = bi_int(n, sign);
	}

	Integer::Integer(const std::string& str, std::size_t size_in_bits)
		: m_Data()

	{

		size_in_bits = (std::size_t)std::ceil((long double)size_in_bits / (sizeof(WORD) * 8));
		if (size_in_bits > 1) {

			Utils::Resize(m_Data, size_in_bits);
			Utils::FromString(m_Data, str);
		}

		else
			Utils::FromString(m_Data, str);
	}

	Integer::Integer(const char* str, std::size_t size_in_bits)
		: m_Data()

	{

		size_in_bits = (std::size_t)std::ceil((long double)size_in_bits / (sizeof(WORD) * 8));
		if (size_in_bits > sizeof(WORD)) {

			Utils::Resize(m_Data, size_in_bits);
			Utils::FromString(m_Data, str);
		}

		else
			Utils::FromString(m_Data, str);
	}

	Integer::Integer(const Integer& other, std::size_t size_in_bits)
		: m_Data()

	{

		size_in_bits = (std::size_t)std::ceil((long double)size_in_bits / (sizeof(WORD) * 8));
		if (size_in_bits > 1) {

			Utils::Resize(m_Data, size_in_bits + other.m_Data.Size);
			Utils::Copy(m_Data, other.m_Data);
		}

		else {

			if (Utils::IsOnStack(other.m_Data))
				Utils::Copy(m_Data, other.m_Data);

			else {

				Utils::Resize(m_Data, other.m_Data.Size);
				Utils::Copy(m_Data, other.m_Data);
			}
		}
	}

	Integer& Integer::operator=(const Integer& other) {

		if (Utils::IsOnStack(other.m_Data))
			Utils::Copy(m_Data, other.m_Data);

		else {

			Utils::Resize(m_Data, other.m_Data.Size);
			Utils::Copy(m_Data, other.m_Data);
		}

		return *this;
	}

	Integer::Integer(Integer&& other) noexcept
		: m_Data(std::move(other.m_Data))

	{}

	Integer::Integer()
		: m_Data()

	{}

	Integer::~Integer() {}

	bool Integer::FromString(Integer& data, const std::string& str) {

		return Utils::FromString(data.m_Data, str);
	}

	std::string Integer::ToString() const {

		return Utils::ToString(m_Data);
	}

	const void* Integer::Data() {

		return m_Data.Buffer;
	}

	std::size_t Integer::Size() const {

		return m_Data.Size;
	}

	std::size_t Integer::SizeInBytes() const {

		return m_Data.Size * sizeof(WORD);
	}

	void Integer::Resize(std::size_t size_in_bits) {

		size_in_bits = (std::size_t)std::ceil((long double)size_in_bits / (sizeof(WORD) * 8));
		Utils::Resize(m_Data, size_in_bits);
	}

	void Integer::ShrinkToFit() {

		Utils::ShrinkToFit(m_Data);
	}

	void Integer::Clear() {

		Utils::Clear(m_Data);
	}

	// Stream

	std::istream& operator>>(std::istream& is, big::Integer& n) {

		std::string str;
		is >> str;

		if (!Integer::FromString(n, str))
			is.setstate(std::ios_base::failbit);

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const big::Integer& n) {

		return os << n.ToString();
	}

	// Operator overloading
}

// --- Big integer structure ---

bi_int::bi_int()
	: Buffer(nullptr), Size(1), Sign(BI_PLUS_SIGN), m_SNO(0)

{

	Buffer = &m_SNO;
}

bi_int::bi_int(WORD sno, bool sign)
	: Buffer(nullptr), Size(1), Sign(sign), m_SNO(sno)

{

	Buffer = &m_SNO;
}

bi_int::bi_int(WORD* buffer, std::size_t size, bool sign)
	: Buffer(buffer), Size(size), Sign(sign), m_SNO(0)

{}

bi_int::bi_int(const bi_int& other)
	: Buffer(nullptr), Size(0), Sign(BI_PLUS_SIGN), m_SNO(0)

{

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &m_SNO;
	}

	else {

		Utils::Resize(*this, other.Size);
		Utils::Copy(*this, other, 0 , false);
	}
}

bi_int::bi_int(bi_int&& other) noexcept
	: Buffer(nullptr), Size(0), Sign(BI_PLUS_SIGN), m_SNO(0)

{

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &m_SNO;
		other.m_SNO = 0;
		other.Sign = BI_PLUS_SIGN;
	}

	else
		Utils::Move(*this, other);
}

bi_int& bi_int::operator=(const bi_int& other) {

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &m_SNO;
	}

	else {

		Utils::Resize(*this, other.Size);
		Utils::Copy(*this, other);
	}

	return *this;
}

bi_int& bi_int::operator=(bi_int&& other) noexcept {

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &m_SNO;
		other.m_SNO = 0;
		other.Sign = BI_PLUS_SIGN;
	}

	else
		Utils::Move(*this, other);

	return *this;
}

bi_int::~bi_int() {

	Utils::Clear(*this);
}
