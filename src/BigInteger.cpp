#include <cassert>
#include <cmath>
#include <random>

#include "Utils.hpp"

#include "BigInteger.hpp"

namespace big {

	void Integer::Rand(big::Integer& n) {

		static std::minstd_rand rand((unsigned)time(NULL));

		for (std::size_t i = 0; i < n.m_Data.Size; i++)
			n.m_Data.Buffer[i] = WORD(rand());
	}

	void Integer::InitFromInt(WORD n, bool sign, std::size_t size) {

		size = (std::size_t)std::ceil((long double)size / (sizeof(WORD) * 8));
		if (size > 1) {

			Utils::Resize(m_Data, size);
			bi_memcpy(m_Data.Buffer, m_Data.Size * sizeof(WORD), &n, sizeof(WORD));
			m_Data.Sign = sign;
		}

		else
			m_Data = BigInt_T(n, sign);
	}

	Integer::Integer(const std::string& str, std::size_t size)
		: m_Data()

	{

		size = (std::size_t)std::ceil((long double)size / (sizeof(WORD) * 8));
		if (size > 1) {

			Utils::Resize(m_Data, size);
			Utils::FromString(m_Data, str);
		}

		else
			Utils::FromString(m_Data, str);
	}

	Integer::Integer(const char* str, std::size_t size)
		: m_Data()

	{

		size = (std::size_t)std::ceil((long double)size / (sizeof(WORD) * 8));
		if (size > 1) {

			Utils::Resize(m_Data, size);
			Utils::FromString(m_Data, str);
		}

		else
			Utils::FromString(m_Data, str);
	}

	Integer::Integer(const Integer& other, std::size_t size)
		: m_Data()

	{

		size = (std::size_t)std::ceil((long double)size / (sizeof(WORD) * 8));
		if (size > 1) {

			Utils::Resize(m_Data, size + other.m_Data.Size);
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

	void Integer::Resize(std::size_t size) {

		size = (std::size_t)std::ceil((long double)size / (sizeof(WORD) * 8));
		Utils::Resize(m_Data, size);
	}

	void Integer::ShrinkToFit() {

		Utils::ShrinkToFit(m_Data);
	}

	void Integer::Clear() {

		Utils::Clear(m_Data);
	}

	// Stream

	BI_API std::istream& operator>>(std::istream& is, big::Integer& n) {

		std::string str;
		is >> str;

		if (!Integer::FromString(n, str))
			is.setstate(std::ios_base::failbit);

		return is;
	}

	BI_API std::ostream& operator<<(std::ostream& os, const big::Integer& n) {

		return os << n.ToString();
	}

	// Arithmetic operators

	BI_API const big::Integer operator+(const big::Integer& a, const big::Integer& b) {

		big::Integer num(a);
		Utils::Add(num.m_Data, b.m_Data);

		return num;
	}

	BI_API const big::Integer operator-(const big::Integer& a, const big::Integer& b) {

		big::Integer num(a);
		Utils::Sub(num.m_Data, b.m_Data);

		return num;
	}

	BI_API const big::Integer operator*(const big::Integer& a, const big::Integer& b) {

		big::Integer num(a);
		Utils::Mult(num.m_Data, b.m_Data);

		return num;
	}

	BI_API const big::Integer operator/(const big::Integer& a, const big::Integer& b) {

		big::Integer num(a);
		Utils::Div(num.m_Data, b.m_Data);

		return num;
	}

	BI_API const big::Integer operator%(const big::Integer& a, const big::Integer& b) {

		throw std::runtime_error("Not implemented yet");

		return a;
	}

	// Relational and comparison operators

	BI_API const bool operator==(const big::Integer& a, const big::Integer& b) {

		return Utils::Compare(a.m_Data, b.m_Data) == 0;
	}

	BI_API const bool operator!=(const big::Integer& a, const big::Integer& b) {

		return Utils::Compare(a.m_Data, b.m_Data) != 0;
	}

	BI_API const bool operator<(const big::Integer& a, const big::Integer& b) {

		return Utils::Compare(a.m_Data, b.m_Data) < 0;
	}

	BI_API const bool operator>(const big::Integer& a, const big::Integer& b) {

		return Utils::Compare(a.m_Data, b.m_Data) > 0;
	}

	BI_API const bool operator<=(const big::Integer& a, const big::Integer& b) {

		return Utils::Compare(a.m_Data, b.m_Data) < 0 || Utils::Compare(a.m_Data, b.m_Data) == 0;
	}

	BI_API const bool operator>=(const big::Integer& a, const big::Integer& b) {

		return Utils::Compare(a.m_Data, b.m_Data) > 0 || Utils::Compare(a.m_Data, b.m_Data) == 0;
	}

	// Logical operators

	big::Integer::operator bool() const {

		return !Utils::IsZero(this->m_Data);
	}

	// Bitwise operators

	BI_API const big::Integer operator&(const big::Integer& a, const big::Integer& b) {

		big::Integer cpy(a);
		Utils::And(cpy.m_Data, b.m_Data);

		return cpy;
	}

	BI_API const big::Integer operator|(const big::Integer& a, const big::Integer& b) {

		big::Integer cpy(a);
		Utils::Or(cpy.m_Data, b.m_Data);

		return cpy;
	}

	BI_API const big::Integer operator^(const big::Integer& a, const big::Integer& b) {

		big::Integer cpy(a);
		Utils::Xor(cpy.m_Data, b.m_Data);

		return cpy;
	}

	BI_API const big::Integer operator~(const big::Integer& n) {

		big::Integer cpy(n);
		Utils::Not(cpy.m_Data);

		return cpy;
	}

	BI_API const big::Integer operator<<(const big::Integer& n, std::size_t bits) {

		big::Integer cpy(n);
		Utils::ShiftLeft(cpy.m_Data, bits);

		return cpy;
	}

	BI_API const big::Integer operator>>(const big::Integer& n, std::size_t bits) {

		big::Integer cpy(n);
		Utils::ShiftRight(cpy.m_Data, bits);

		return cpy;
	}

	BI_API const big::Integer operator<<(const big::Integer& n, big::Integer bits) {

		big::Integer cpy(n);
		while (Utils::CountSignificantWords(bits.m_Data) != 1) {

			Utils::ShiftLeft(cpy.m_Data, BI_MAX_WORD);
			bits -= BI_MAX_WORD;
		}

		Utils::ShiftLeft(cpy.m_Data, bits.m_Data.Buffer[0]);

		return cpy;
	}

	BI_API const big::Integer operator>>(const big::Integer& n, big::Integer bits) {

		big::Integer cpy(n);
		while (Utils::CountSignificantWords(bits.m_Data) != 1) {

			Utils::ShiftRight(cpy.m_Data, BI_MAX_WORD);
			bits -= BI_MAX_WORD;
		}

		Utils::ShiftRight(cpy.m_Data, bits.m_Data.Buffer[0]);

		return cpy;
	}

	// Assignment operators

	BI_API big::Integer& operator+=(big::Integer& a, const big::Integer& b) {

		Utils::Add(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator-=(big::Integer& a, const big::Integer& b) {

		Utils::Sub(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator*=(big::Integer& a, const big::Integer& b) {

		Utils::Mult(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator/=(big::Integer& a, const big::Integer& b) {

		Utils::Div(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator%=(big::Integer& a, const big::Integer& b) {

		throw std::runtime_error("Not implemented yet");

		return a;
	}

	BI_API big::Integer& operator&=(big::Integer& a, const big::Integer& b) {

		Utils::And(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator|=(big::Integer& a, const big::Integer& b) {

		Utils::Or(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator^=(big::Integer& a, const big::Integer& b) {

		Utils::Xor(a.m_Data, b.m_Data);

		return a;
	}

	BI_API big::Integer& operator<<=(big::Integer& n, std::size_t bits) {

		Utils::ShiftLeft(n.m_Data, bits);

		return n;
	}

	BI_API big::Integer& operator>>=(big::Integer& n, std::size_t bits) {

		Utils::ShiftRight(n.m_Data, bits);

		return n;
	}

	BI_API big::Integer& operator<<=(big::Integer& n, big::Integer bits) {

		while (Utils::CountSignificantWords(bits.m_Data) != 1) {

			Utils::ShiftLeft(n.m_Data, BI_MAX_WORD);
			bits -= BI_MAX_WORD;
		}

		Utils::ShiftLeft(n.m_Data, bits.m_Data.Buffer[0]);

		return n;
	}

	BI_API big::Integer& operator>>=(big::Integer& n, big::Integer bits) {

		while (Utils::CountSignificantWords(bits.m_Data) != 1) {

			Utils::ShiftRight(n.m_Data, BI_MAX_WORD);
			bits -= BI_MAX_WORD;
		}

		Utils::ShiftRight(n.m_Data, bits.m_Data.Buffer[0]);

		return n;
	}

	// Unary operators

	BI_API big::Integer& operator+(big::Integer& n) {

		return n;
	}

	BI_API big::Integer& operator++(big::Integer& n) {

		Utils::Increment(n.m_Data);

		return n;
	}

	BI_API big::Integer operator++(big::Integer& n, int) {

		big::Integer cpy(n);
		Utils::Increment(n.m_Data);

		return cpy;
	}

	BI_API big::Integer& operator-(big::Integer& n) {

		Utils::Negate(n.m_Data);

		return n;
	}

	BI_API big::Integer& operator--(big::Integer& n) {

		Utils::Decrement(n.m_Data);

		return n;
	}

	BI_API big::Integer operator--(big::Integer& n, int) {

		big::Integer cpy(n);
		Utils::Decrement(n.m_Data);

		return cpy;
	}
}

// --- Big integer structure ---

BigInt_T::BigInt_T()
	: Buffer(nullptr), Size(1), Sign(BI_PLUS_SIGN), SNO(0)

{

	Buffer = &SNO;
}

BigInt_T::BigInt_T(WORD sno, bool sign)
	: Buffer(nullptr), Size(1), Sign(sign), SNO(sno)

{

	Buffer = &SNO;
}

BigInt_T::BigInt_T(WORD* buffer, std::size_t size, bool sign)
	: Buffer(buffer), Size(size), Sign(sign), SNO(0)

{}

BigInt_T::BigInt_T(const BigInt_T& other)
	: Buffer(nullptr), Size(0), Sign(BI_PLUS_SIGN), SNO(0)

{

	if (Utils::IsOnStack(other)) {

		SNO = other.SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &SNO;
	}

	else {

		Utils::Resize(*this, other.Size);
		Utils::Copy(*this, other);
	}
}

BigInt_T::BigInt_T(BigInt_T&& other) noexcept
	: Buffer(nullptr), Size(0), Sign(BI_PLUS_SIGN), SNO(0)

{

	if (Utils::IsOnStack(other)) {

		SNO = other.SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &SNO;
		other.SNO = 0;
		other.Sign = BI_PLUS_SIGN;
	}

	else
		Utils::Move(*this, other);
}

BigInt_T& BigInt_T::operator=(const BigInt_T& other) {

	if (Utils::IsOnStack(other)) {

		SNO = other.SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &SNO;
	}

	else {

		Utils::Resize(*this, other.Size);
		Utils::Copy(*this, other);
	}

	return *this;
}

BigInt_T& BigInt_T::operator=(BigInt_T&& other) noexcept {

	if (Utils::IsOnStack(other)) {

		SNO = other.SNO;
		Sign = other.Sign;
		Size = other.Size;
		Buffer = &SNO;
		other.SNO = 0;
		other.Sign = BI_PLUS_SIGN;
	}

	else
		Utils::Move(*this, other);

	return *this;
}

BigInt_T::~BigInt_T() {

	Utils::Clear(*this);
}
