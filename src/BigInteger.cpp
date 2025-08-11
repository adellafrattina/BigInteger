#include <cassert>
#include <cmath>

#include "Utils.hpp"

#include "BigInteger.hpp"

namespace big {

	Integer::Integer(std::int64_t n, std::size_t size_in_bytes)
		: m_Data()

	{

		if (size_in_bytes > sizeof(WORD)) {

			Utils::Resize(m_Data, size_in_bytes, false);
			bi_int tmp = Utils::BigIntegerFromWORD(n);
			Utils::Copy(m_Data, tmp);
		}

		else
			m_Data = Utils::BigIntegerFromWORD(n);
	}

	Integer::Integer(const std::string& str, std::size_t size_in_bytes)
		: m_Data()

	{

		if (size_in_bytes > sizeof(WORD)) {

			Utils::Resize(m_Data, size_in_bytes, false);
			Utils::FromString(m_Data, str);
		}

		else
			Utils::FromString(m_Data, str);
	}

	Integer::Integer(const char* str, std::size_t size_in_bytes)
		: m_Data()

	{

		if (size_in_bytes > sizeof(WORD)) {

			Utils::Resize(m_Data, size_in_bytes, false);
			Utils::FromString(m_Data, str);
		}

		else
			Utils::FromString(m_Data, str);
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

	Integer& Integer::operator=(const Integer& other) {

		if (Utils::IsOnStack(other.m_Data))
			m_Data = Utils::BigIntegerFromWORD(GetSNO(other.m_Data));

		else {

			Utils::Resize(m_Data, other.m_Data.Size, false);
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

		return m_Data.Size * sizeof(bi_type);
	}

	void Integer::Resize(std::size_t size_in_bytes, bool ext_sign) {

		Utils::Resize(m_Data, size_in_bytes, ext_sign);
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

	big::Integer operator+(const big::Integer& first, const big::Integer& second) {

		big::Integer res;
		Utils::Resize(res.m_Data, std::max(first.m_Data.Size, second.m_Data.Size), false);
		Utils::Copy(res.m_Data, first.m_Data);
		Utils::Add(res.m_Data, second.m_Data);

		return res;
	}

	big::Integer& operator+=(big::Integer& first, const big::Integer& second) {

		Utils::Add(first.m_Data, second.m_Data);

		return first;
	}
}

// --- Big integer structure ---

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
	: Buffer(nullptr), Size(0), m_SNO(0)

{

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Size = other.Size;
		Buffer = reinterpret_cast<bi_type*>(&m_SNO);
	}

	else {

		Utils::Resize(*this, other.Size, false);
		Utils::Copy(*this, other, 0 , false);
	}
}

bi_int::bi_int(bi_int&& other) noexcept
	: Buffer(nullptr), Size(0), m_SNO(0)

{

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Size = other.Size;
		Buffer = reinterpret_cast<bi_type*>(&m_SNO);
		other.m_SNO = 0;
		memset(other.Buffer, 0, other.Size);
	}

	else
		Utils::Move(*this, other);
}

bi_int& bi_int::operator=(const bi_int& other) {

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Size = other.Size;
		Buffer = reinterpret_cast<bi_type*>(&m_SNO);
	}

	else {

		Utils::Resize(*this, other.Size, false);
		Utils::Copy(*this, other, 0, false);
	}

	return *this;
}

bi_int& bi_int::operator=(bi_int&& other) noexcept {

	if (Utils::IsOnStack(other)) {

		m_SNO = other.m_SNO;
		Size = other.Size;
		Buffer = reinterpret_cast<bi_type*>(&m_SNO);
		other.m_SNO = 0;
		memset(other.Buffer, 0, other.Size);
	}

	else
		Utils::Move(*this, other);

	return *this;
}

bi_int::~bi_int() {

	Utils::Clear(*this);
}
