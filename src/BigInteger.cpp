#include <cassert>

#include "Utils.hpp"

#include "BigInteger.hpp"

using namespace Utils;

constexpr std::uint8_t HIGH_BITS = 0xF0;
constexpr std::uint8_t LOW_BITS = 0x0F;

namespace bi {

	Integer::Integer(const std::string& str)
		: m_Data(nullptr), m_Size(0)

	{

		Init(str);
	}

	Integer::Integer(std::uint64_t n)
		: m_Data(nullptr), m_Size(0)

	{

		Init(n);
	}

	Integer::Integer(const Integer& other)
		: m_Data(new bi_int[other.m_Size]), m_Size(other.m_Size)

	{

		memcpy_s(m_Data, m_Size * sizeof(bi_int), other.m_Data, other.m_Size * sizeof(bi_int));
	}

	Integer::Integer(Integer&& other) noexcept
		: m_Data(other.m_Data), m_Size(other.m_Size)

	{

		other.m_Data = nullptr;
		other.m_Size = 0;
	}

	Integer::Integer()
		: m_Data(nullptr), m_Size(0)

	{


	}

	Integer::~Integer() {

		Clear();
	}

	std::string Integer::ToString() const {

		// Double dabble algorithm

		// Size in bytes = ceil(4*ceil(n/3)/8) + 1
		// The last one is needed as an auxiliary buffer to store the first 8 bits in the number
		const std::size_t bcdBufferSize = ((m_Size * sizeof(bi_int) * 8) / 3 + 1) + 1;

		// The bcd buffer start
		const std::size_t bcdBufferStart = ((m_Size * sizeof(bi_int) * 8) / 3 + 1) / 2;

		// The binary-coded decimal buffer
		std::uint8_t* bcdBuffer = (std::uint8_t*)calloc(bcdBufferSize, sizeof(std::uint8_t));
		if (bcdBuffer == NULL)
			return "0";

		std::uint8_t* buffer = (std::uint8_t*)m_Data;
		for (std::size_t shift = 0; shift < m_Size * sizeof(bi_int) * 8; shift++) {

			// If the shift counter consumes all the bits in the auxiliar buffer, refill it with new data
			if (shift % 8 == 0) {

				bcdBuffer[bcdBufferSize - 1] = buffer[m_Size * sizeof(bi_int) - 1 - shift / 8];
			}

			for (std::size_t i = bcdBufferStart; i < bcdBufferSize - 1; i++) {

				if (bcdBuffer[i] == 0)
					continue;

				// Check the high bits...

				std::uint8_t digit = (bcdBuffer[i] & HIGH_BITS) >> 4;
				if (digit > 4) {

					digit += 3;
					bcdBuffer[i] = (digit << 4) | (LOW_BITS & bcdBuffer[i]);
				}

				// and the low bits

				digit = bcdBuffer[i] & LOW_BITS;
				if (digit > 4) {

					digit += 3;
					bcdBuffer[i] = digit | (HIGH_BITS & bcdBuffer[i]);
				}
			}

			// Shift left by one position
			ShiftLeft1(bcdBuffer, bcdBufferSize);
		}

		// Shift the bits in the correct position to create the string buffer
		for (std::size_t i = 0; i < bcdBufferSize - 1; i++) {

			ShiftLeft(bcdBuffer, bcdBufferSize - 1 - i, 4);
			bcdBuffer[bcdBufferSize - 2 - i] >>= 4;
		}

		// Remove the useless bits at the start
		std::size_t offset = 0;
		for (; offset < bcdBufferSize - 1; offset++)
			if (bcdBuffer[offset] != 0)
				break;

		// Create the actual digit string
		std::string data((char*)(bcdBuffer + offset), bcdBufferSize - 1 - offset);
		for (std::size_t i = 0; i < data.size(); i++)
			data[i] = data[i] + 48; // Convert from binary number to the ASCII character number

		// Free the bcd buffer
		free(bcdBuffer);

		return data;
	}

	const void* Integer::Data() {

		return m_Data;
	}

	std::size_t Integer::Size() const {

		return m_Size;
	}

	std::size_t Integer::SizeInBytes() const {

		return m_Size * sizeof(bi_int);
	}

	Integer& Integer::operator=(const std::string& str) {

		Init(str);

		return *this;
	}

	Integer& Integer::operator=(std::uint64_t n) {

		Init(n);

		return *this;
	}

	Integer& Integer::operator=(const Integer& other) {

		assert((void("Not implemented yet"), false));

		m_Data = other.m_Data;

		return *this;
	}

	Integer Integer::operator+(std::uint64_t n) {

		Integer addend(n);
		Integer new_int(*this);
		new_int.m_Size = Add(new_int.m_Data, new_int.m_Size, addend.m_Data, addend.m_Size);

		return new_int;
	}

	Integer Integer::operator+=(std::uint64_t n) {

		Integer addend(n);
		m_Size = Add(m_Data, m_Size, addend.m_Data, addend.m_Size);

		return *this;
	}

	Integer Integer::operator+(const Integer& other) {

		Integer new_int(*this);
		new_int.m_Size = Add(new_int.m_Data, new_int.m_Size, other.m_Data, other.m_Size);

		return new_int;
	}

	Integer Integer::operator+=(const Integer& other) {

		m_Size = Add(m_Data, m_Size, other.m_Data, other.m_Size);

		return *this;
	}

	Integer Integer::operator++() {

		m_Size = Increment(m_Data, m_Size);

		return *this;
	}

	Integer Integer::operator-(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return Integer(0);
	}

	Integer Integer::operator*(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return Integer(0);
	}

	Integer Integer::operator/(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return Integer(0);
	}

	bool Integer::operator==(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return m_Data == other.m_Data;
	}

	bool Integer::operator!=(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return m_Data != other.m_Data;
	}

	bool Integer::operator>(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return m_Data > other.m_Data;
	}

	bool Integer::operator<(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return m_Data < other.m_Data;
	}

	bool Integer::operator>=(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return m_Data >= other.m_Data;
	}

	bool Integer::operator<=(const Integer& other) {

		assert((void("Not implemented yet"), false));

		return m_Data <= other.m_Data;
	}

	std::istream& operator>>(std::istream& is, bi::Integer& n) {

		std::string str;
		is >> str;
		if (!n.Init(str))
			is.setstate(std::ios_base::failbit);

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const bi::Integer& n) {

		return os << n.ToString();
	}

	bool Integer::Init(const std::string& str) {

		if (str.empty())
			return false;

		// Check if the number is positive or negative
		const bool isNegative = str.at(0) == '-';
		const std::size_t strLength = isNegative ? str.length() - 1 : str.length();
		if (strLength == 0)
			return false;

		// Check the string is made up by numbers only
		for (std::size_t i = isNegative; i < str.length(); i++)
			if (str.at(i) < '0' || str.at(i) > '9')
				return false;

		// Reverse double dabble algorithm

		// Size in bytes = ceil(string length / 2) + 1
		// The last one is needed as an auxiliary buffer to store the first 8 bits in the final number
		const std::size_t bcdBufferSize = (std::size_t)(std::ceil((long double)strLength / 2.0l)) + 1;

		// The binary-coded decimal buffer
		std::uint8_t* bcdBuffer = (std::uint8_t*)calloc(bcdBufferSize, sizeof(std::uint8_t));
		if (bcdBuffer == NULL)
			return "0";

		// Fill the bcd buffer with the provided data.
		// It starts at the end to ensure the last bits are adjacent to the auxiliary buffer,
		// so the algorithm can right shift them in the correct position

		long double i = 0.0;
		std::size_t strIndex = 0;
		std::uint8_t nibble_offset = 0x0F;
		std::uint8_t shiftAmount = 0;
		while (strIndex < strLength) {

			bcdBuffer[bcdBufferSize - 2 - (std::size_t)i] |= ((std::uint8_t)(str.at(strLength - 1 - strIndex + isNegative) - '0') << shiftAmount) & nibble_offset;
			nibble_offset = ~nibble_offset;
			shiftAmount = shiftAmount == 4 ? 0 : 4;
			i = i + 0.5;
			strIndex++;
		}

		// Set up data
		Clear();
		m_Size = (std::size_t)std::ceil(std::ceil((long double)strLength * log2(10.0l)) / (sizeof(bi_int) * 8.0l));
		Resize(m_Data, 0, m_Size);

		std::size_t offset = 0;
		std::uint8_t* buffer = (std::uint8_t*)m_Data;
		for (std::size_t shift = 0; shift < m_Size * sizeof(bi_int) * 8 + 1; shift++) { // Plus one because we need to shift the last bit into the auxiliary buffer

			// When we have shifted 8 bits in the auxiliar buffer, transfer it to the buffer
			if (shift > 0 && shift % 8 == 0) {

				buffer[shift / 8 - 1] = bcdBuffer[bcdBufferSize - 1];
				offset++;
			}

			// Shift right by one position
			ShiftRight1(bcdBuffer, bcdBufferSize);

			// We start from an offset to avoid checking values that have already been processed
			for (std::size_t i = offset; i < bcdBufferSize - 1; i++) {

				if (bcdBuffer[i] == 0)
					continue;

				// Check the high bits...

				std::uint8_t digit = (bcdBuffer[i] & HIGH_BITS) >> 4;
				if (digit > 4) {

					digit -= 3;
					bcdBuffer[i] = (digit << 4) | (LOW_BITS & bcdBuffer[i]);
				}

				// and the low bits

				digit = bcdBuffer[i] & LOW_BITS;
				if (digit > 4) {

					digit -= 3;
					bcdBuffer[i] = digit | (HIGH_BITS & bcdBuffer[i]);
				}
			}
		}

		return true;
	}

	void Integer::Init(const std::uint64_t& n) {

		Clear();

		m_Data = new bi_int[2];
		m_Size = 2;

		const std::uint64_t* ptr = &n;
		std::size_t pos = 56;
		std::uint8_t* buffer = (std::uint8_t*)m_Data;
		for (std::size_t i = 0; i < m_Size * sizeof(bi_int); i++) {

			buffer[m_Size * sizeof(bi_int) - 1 - i] = static_cast<std::uint8_t>((n >> pos) & 0xFF);
			pos -= 8;
		}
	}

	void Integer::Clear() {

		if (m_Data != nullptr) {

			PRINT("Clear called (data: %p, size: %zu)", m_Data, m_Size);
			delete[] m_Data;
		}

		m_Data = nullptr;
		m_Size = 0;
	}
}
