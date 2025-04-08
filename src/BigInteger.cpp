#include <cassert>

#define BI_PRINT_ACTIVE
#include "Utils.hpp"

#include "BigInteger.hpp"

using namespace Utils;

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
		: m_Data(new std::uint32_t[other.m_Size]), m_Size(other.m_Size)

	{

		memcpy_s(m_Data, m_Size * sizeof(std::uint32_t), other.m_Data, other.m_Size * sizeof(std::uint32_t));
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
		const std::size_t bcdBufferSize = ((m_Size * 32) / 3 + 1) / 2 + 1;

		// The binary-coded decimal buffer
		std::uint8_t* bcdBuffer = (std::uint8_t*)calloc(bcdBufferSize, sizeof(std::uint8_t));
		if (bcdBuffer == NULL)
			return "0";

		std::uint8_t* buffer = (std::uint8_t*)m_Data;

		// Set data in the auxiliary buffer
		bcdBuffer[bcdBufferSize - 1] = buffer[m_Size * 4 - 1];

		std::uint8_t shiftCounter = 0;
		std::size_t dataIndex = m_Size * 4 - 2;
		for (std::size_t shift = 0; shift < m_Size * 32; shift++) {

			// If the shift counter consumes all the bits in the auxiliar buffer, refill it with new data
			if (shiftCounter > 7) {

				bcdBuffer[bcdBufferSize - 1] = buffer[dataIndex--];
				shiftCounter = 0;
			}

			for (std::size_t i = 0; i < bcdBufferSize - 1; i++) {

				if (bcdBuffer[i] == 0)
					continue;

				constexpr std::uint8_t high = 0b11110000;
				constexpr std::uint8_t low = 0b00001111;

				// Check the high bits...

				std::uint8_t digit = (bcdBuffer[i] & high) >> 4;
				if (digit > 4) {

					digit += 3;
					bcdBuffer[i] = (digit << 4) | (low & bcdBuffer[i]);
				}

				// and the low bits

				digit = bcdBuffer[i] & low;
				if (digit > 4) {

					digit += 3;
					bcdBuffer[i] = digit | (high & bcdBuffer[i]);
				}
			}

			// Shift left by one position
			ShiftLeft1(bcdBuffer, bcdBufferSize);
			shiftCounter++;
		}

		// Remove the useless bit at the start

		std::size_t byte_offset = 0;
		std::uint8_t nibble_offset = 0b00001111;
		while (byte_offset < bcdBufferSize - 1) {

			if ((bcdBuffer[byte_offset] & 0b11110000) != 0) {

				nibble_offset = 0b11110000;
				break;
			}

			if ((bcdBuffer[byte_offset] & 0b00001111) != 0) {

				nibble_offset = 0b00001111;
				break;
			}

			byte_offset++;
		}

		// If the number is zero, read the last value
		if (bcdBufferSize - 1 - byte_offset == 0)
			byte_offset--;

		std::string data;
		data.resize((bcdBufferSize - 1 - byte_offset) * 2 - (nibble_offset & 0x01));

		// Convert each nibble in the bcd buffer and insert it in the string 'data'
		for (std::size_t i = 0; i < data.size(); i++) {

			std::uint8_t digit = (bcdBuffer[byte_offset] & nibble_offset);

			if (nibble_offset == 0b00001111)
				byte_offset++;

			else
				digit >>= 4;

			data[i] = ((char)(digit + 48));
			nibble_offset = ~nibble_offset;
		}

		free(bcdBuffer);

		return data;
	}

	const void* Integer::Data() {

		return m_Data;
	}

	std::size_t Integer::Size() const {

		return m_Size;
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

	Integer Integer::operator+(const Integer& other) {

		Integer new_int(*this);
		new_int.m_Size = Add(new_int.m_Data, new_int.m_Size, other.m_Data, other.m_Size);

		return new_int;
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

	Integer::Integer(std::uint32_t* data, std::size_t size)
		: m_Data(data), m_Size(size)

	{}

	bool Integer::Init(const std::string& str) {

		if (str.empty())
			return false;

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

		std::size_t index = 0;
		for (std::size_t i = 0; i < bcdBufferSize - 1; i++) {

			std::uint8_t digit = (std::uint8_t)(str.at(strLength - 1 - index + isNegative) - '0');
			bcdBuffer[bcdBufferSize - 2 - i] |= digit & 0x0F;

			if (index + 2 <= strLength) {

				digit = (std::uint8_t)(str.at(strLength - 2 - index + isNegative) - '0');
				bcdBuffer[bcdBufferSize - 2 - i] |= (digit << 4) & 0xF0;
			}

			index += 2;
		}

		// Set up data
		Clear();
		m_Size = (std::size_t)std::ceil(std::ceil((long double)strLength * log2(10)) / 32.0l);
		Resize(m_Data, 0, m_Size);

		std::uint8_t shiftCounter = 0;
		//std::size_t dataIndex = m_Size * 4 - 1;
		std::size_t dataIndex = 0;
		std::uint8_t* buffer = (std::uint8_t*)m_Data;
		for (std::size_t shift = 0; shift < m_Size * 32 + 1; shift++) { // Don't know why it works, but it does... :)

			// If the shift counter consumes all the bits in the auxiliar buffer, refill it with new data
			if (shiftCounter > 7) {

				buffer[dataIndex++] = bcdBuffer[bcdBufferSize - 1];
				shiftCounter = 0;
			}

			// Shift right by one position
			ShiftRight1(bcdBuffer, bcdBufferSize);
			shiftCounter++;

			for (std::size_t i = 0; i < bcdBufferSize - 1; i++) {

				if (bcdBuffer[i] == 0)
					continue;

				constexpr std::uint8_t high = 0b11110000;
				constexpr std::uint8_t low = 0b00001111;

				// Check the high bits...

				std::uint8_t digit = (bcdBuffer[i] & high) >> 4;
				if (digit > 4) {

					digit -= 3;
					bcdBuffer[i] = (digit << 4) | (low & bcdBuffer[i]);
				}

				// and the low bits

				digit = bcdBuffer[i] & low;
				if (digit > 4) {

					digit -= 3;
					bcdBuffer[i] = digit | (high & bcdBuffer[i]);
				}
			}
		}

		return true;
	}

	void Integer::Init(const std::uint64_t& n) {

		Clear();

		m_Data = new std::uint32_t[2];
		m_Size = 2;

		const std::uint64_t* ptr = &n;
		std::size_t pos = 56;
		std::uint8_t* buffer = (std::uint8_t*)m_Data;
		for (std::size_t i = 0; i < m_Size * 4; i++) {

			buffer[m_Size * 4 - 1 - i] = static_cast<std::uint8_t>((n >> pos) & 0xFF);
			pos -= 8;
		}
	}

	void Integer::Clear() {

		PRINT("Clear called (data: %p, size: %zu)", m_Data, m_Size);

		if (m_Data != nullptr)
			delete[] m_Data;
		m_Data = nullptr;
		m_Size = 0;
	}
}
