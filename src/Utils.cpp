#include <cmath>

#include "Utils.hpp"

#undef BI_WORD_SIZE
#define BI_WORD_SIZE sizeof(WORD) / sizeof(bi_type)

#undef BI_SIGN_BUFFER
#define BI_SIGN_BUFFER(b, s) (b[s - 1] & ((bi_type)1 << (sizeof(bi_type) * 8 - 1))) ? BI_MINUS_SIGN : BI_PLUS_SIGN
#undef BI_SIGN
#define BI_SIGN(x) BI_SIGN_BUFFER(x.Buffer, x.Size)

WORD GetSNO(const bi_int& data) {

	return Utils::BigIntegerToWORD(data);
}

void SetSNO(bi_int& data, WORD sno) {

	data = Utils::BigIntegerFromWORD(sno);
}

namespace Utils {

	// --- Debug functions ---

	void PrintAsBinary(void* data, std::size_t size_in_bytes) {

		std::uint8_t* buffer = (std::uint8_t*)data;

		for (std::size_t i = 0; i < size_in_bytes; i++) {

			std::uint8_t mask;
			for (mask = 1 << (CHAR_BIT - 1); mask; mask >>= 1)
				putchar(mask & buffer[size_in_bytes - 1 - i] ? '1' : '0');
		}

		putchar('\n');
	}

	// --- Basic functions ---

	inline bool IsLittleEndian() {

		std::uint16_t n = 1;
		std::uint8_t* ptr = (std::uint8_t*)&n;

		return ptr[0] == 1;
	}

	bool IsOnStack(const bi_int& data) {

		return data.Size == BI_WORD_SIZE;
	}

	void Resize(bi_int& data, std::size_t new_size, bool ext_sign) {

		const std::size_t old_size = data.Size;
		new_size = new_size <= BI_WORD_SIZE ? BI_WORD_SIZE : new_size;

		// There is no point in executing a resize if both sizes are equal
		if (old_size == new_size)
			return;

		PRINT("Resize called (data: %p, old_size: %zu, new_size: %zu)", data.Buffer, old_size, new_size);

		// This means that the big integer buffer was originally allocated on the heap, but now it will be allocated on the stack
		if (new_size == BI_WORD_SIZE) {

			bi_type* tmp = data.Buffer; // Data was allocated on the heap
			SetSNO(data, BigIntegerToWORD(data)); // Allocate on the stack
			delete[] tmp; // Free the heap memory
		}

		// Regardless of how the big integer buffer was allocated, now it will be allocated on the heap
		else {

			bi_type* tmp = data.Buffer; // Previous allocated data
			data.Buffer = new bi_type[new_size]; // Allocate on the heap
			memset(data.Buffer, 0, new_size * sizeof(bi_type));

			// Copy the old data
			bi_memcpy(data.Buffer, new_size * sizeof(bi_type), tmp, (old_size > new_size ? new_size : old_size) * sizeof(bi_type));

			// Fill the rest of the new buffer data with the old data sign
			if (ext_sign)
				for (std::size_t i = old_size; i < new_size; i++)
					data.Buffer[i] = BI_SIGN_BUFFER(tmp, old_size);

			data.Size = new_size;

			// If the big integer buffer was allocated on the heap, delete the old memory
			if (old_size > BI_WORD_SIZE)
				delete[] tmp;
		}
	}

	void Copy(bi_int& dest, const bi_int& src, const std::size_t offset_dest, bool ext_sign) {

		bi_memcpy(dest.Buffer + offset_dest, dest.Size * sizeof(bi_type), src.Buffer, src.Size * sizeof(bi_type));

		if (ext_sign) {

			const bi_type sign = BI_SIGN(src);
			for (std::size_t i = src.Size; i < dest.Size; i++)
				dest.Buffer[i] = sign;
		}
	}

	void Move(bi_int& dest, bi_int& src, bool ext_sign) {

		if (IsOnStack(src))
			Copy(dest, src, 0, ext_sign);

		else {

			dest.Buffer = src.Buffer;
			dest.Size = src.Size;
			SetSNO(src, 0);
		}
	}

	void Clear(bi_int& data) {

		if (data.Size != BI_WORD_SIZE)
			delete[] data.Buffer;

		SetSNO(data, 0);
	}

	void ShrinkToFit(bi_int& data) {

		if (data.Size == BI_WORD_SIZE)
			return;

		const bi_type sign = BI_SIGN(data);

		std::size_t size = BI_WORD_SIZE;
		for (std::size_t i = 0; i < data.Size; i++) {

			if (data.Buffer[data.Size - 1 - i] != sign || data.Size - i == BI_WORD_SIZE) {

				const bi_type currentCellSign = BI_SIGN_BUFFER(data.Buffer, data.Size - i);
				if (currentCellSign == sign)
					size = data.Size - i;
				else
					size = data.Size + 1 - i;

				break;
			}
		}

		Resize(data, size, false);
	}

	std::size_t GetBitSize(WORD n) {

		std::int32_t bits = 1;
		while (n > 0) {

			n >>= 1;
			++bits;
		}

		return bits;
	}

	std::size_t GetByteSize(WORD n) {

		return (std::size_t)std::ceil((long double)(GetBitSize(n)) / 8.0);
	}

	inline WORD BigIntegerToWORD(const bi_int& data) {

		const std::uint8_t* buffer = reinterpret_cast<const std::uint8_t*>(data.Buffer);

		if (IsLittleEndian())
			return *reinterpret_cast<const WORD*>(buffer);

		else {

			WORD word = 0;
			std::uint8_t* wordBuffer = reinterpret_cast<std::uint8_t*>(&word);
			for (std::size_t i = 0; i < sizeof(WORD); i++)
				wordBuffer[i] = buffer[sizeof(WORD) - 1 - i];

			return word;
		}
	}

	inline bi_int BigIntegerFromWORD(WORD word) {

		if (IsLittleEndian())
			return { word };

		else {

			bi_int data;
			const std::uint8_t* wordBuffer = reinterpret_cast<const std::uint8_t*>(&word);
			std::uint8_t* buffer = reinterpret_cast<std::uint8_t*>(data.Buffer);
			for (std::size_t i = 0; i < data.Size * sizeof(bi_type); i++)
				buffer[i] = wordBuffer[sizeof(WORD) - 1 - i];

			return data;
		}
	}

	WORD BytesToWORD(const std::uint8_t* data, std::size_t size_in_bytes) {

		WORD word = 0;
		size_in_bytes = size_in_bytes > sizeof(WORD) ? sizeof(WORD) : size_in_bytes;
		if (IsLittleEndian()) {

			bi_memcpy(&word, sizeof(word), data, size_in_bytes);
		}

		else {

			std::uint8_t* wordBuffer = reinterpret_cast<std::uint8_t*>(&word);
			for (std::size_t i = 0; i < sizeof(WORD); i++)
				wordBuffer[i] = data[size_in_bytes - 1 - i];
		}

		return word;
	}

	void BytesFromWORD(std::uint8_t* data, std::size_t size_in_bytes, const WORD word) {

		size_in_bytes = size_in_bytes > sizeof(WORD) ? sizeof(WORD) : size_in_bytes;
		if (IsLittleEndian()) {

			bi_memcpy(data, size_in_bytes, &word, size_in_bytes);
		}

		else {

			const std::uint8_t* wordBuffer = reinterpret_cast<const std::uint8_t*>(&word);
			for (std::size_t i = 0; i < size_in_bytes; i++)
				data[size_in_bytes - 1 - i] = wordBuffer[i];
		}
	}

	inline bool IsNegative(const bi_int& data) {

		return BI_SIGN(data) == BI_MINUS_SIGN;
	}

	std::size_t CountSignificantBits(const bi_type* const data, std::size_t size) {

		std::size_t bitSize = 0;
		const bi_type sign = BI_SIGN_BUFFER(data, size);

		for (std::size_t i = 0; i < size; i++) {

			if (data[size - 1 - i] != sign) {

				const bi_type currentCellSign = BI_SIGN_BUFFER(data, size - i);
				const std::size_t index = currentCellSign == sign ? size - 1 - i : size - i;
				bi_type min = data[index];
				bitSize = index * 8;
				bitSize += min > 0 ? (std::size_t)log2(min) + 1 : 1;
				break;
			}
		}

		return bitSize == 0 ? 1 : bitSize;
	}

	// --- Mathematical functions ---

	int Compare(const bi_int& first, const bi_int& second) {

		const bi_type sign1 = BI_SIGN(first);
		const bi_type sign2 = BI_SIGN(second);

		// If the first one is positive and the second one is negative, then return 1, else return -1
		if (sign1 < sign2)
			return 1;
		else if (sign1 > sign2)
			return -1;

		const std::size_t firstBitSize = CountSignificantBits(first.Buffer, first.Size);
		const std::size_t secondBitSize = CountSignificantBits(second.Buffer, second.Size);

		// If the number of significant bits in the first number is higher than the second one, then return 1, else return -1
		if (firstBitSize > secondBitSize)
			return 1;
		else if (firstBitSize < secondBitSize)
			return -1;

		std::size_t size = std::min(first.Size, second.Size) - 1;

		// Check every byte in the two numbers
		do {

			if (first.Buffer[size] > second.Buffer[size])
				return 1;
			else if (first.Buffer[size] < second.Buffer[size])
				return -1;

		} while (size--);

		return 0;
	}

	void Negate(bi_int& data) {

		Not(data);
		Increment(data);
	}

	void Abs(bi_int& data) {

		if (IsNegative(data))
			Negate(data);
	}

	void Increment(bi_int& data) {

		const bool sameSign = BI_SIGN(data) == BI_PLUS_SIGN;
		const std::size_t sizeAsWords = data.Size / sizeof(WORD);
		const std::size_t remainingBytes = data.Size % sizeof(WORD);

		std::uint8_t carry = 1;
		std::size_t i = 0;
		while (i < sizeAsWords && carry != 0) {

			carry = 0;
			WORD value = BytesToWORD(data.Buffer + i * sizeof(WORD), sizeof(WORD));
			BytesFromWORD(data.Buffer + i * sizeof(WORD), sizeof(WORD), value + 1);
			if (value + 1 <= value)
				carry = 1;

			i++;
		}

		while (i < remainingBytes && carry != 0) {

			carry = 0;
			bi_type value = data.Buffer[i];
			data.Buffer[i]++;
			if (data.Buffer[i] <= value)
				carry = 1;

			i++;
		}

		if (sameSign) {

			if (BI_SIGN(data) != BI_PLUS_SIGN) {

				Resize(data, data.Size + 1, false);
				data.Buffer[data.Size - 1] = BI_PLUS_SIGN;
			}
		}
	}

	void Decrement(bi_int& data) {

		const bool sameSign = BI_SIGN(data) == BI_MINUS_SIGN;
		const std::size_t sizeAsWords = data.Size / sizeof(WORD);
		const std::size_t remainingBytes = data.Size % sizeof(WORD);

		std::uint8_t carry = 1;
		std::size_t i = 0;
		while (i < sizeAsWords && carry != 0) {

			carry = 0;
			WORD value = BytesToWORD(data.Buffer + i * sizeof(WORD), sizeof(WORD));
			BytesFromWORD(data.Buffer + i * sizeof(WORD), sizeof(WORD), value - 1);
			if (value - 1 >= value)
				carry = 1;

			i++;
		}

		while (i < remainingBytes && carry != 0) {

			carry = 0;
			bi_type value = data.Buffer[i];
			data.Buffer[i]--;
			if (data.Buffer[i] >= value)
				carry = 1;

			i++;
		}

		if (sameSign) {

			if (BI_SIGN(data) != BI_MINUS_SIGN) {

				Resize(data, data.Size + 1, false);
				data.Buffer[data.Size - 1] = BI_MINUS_SIGN;
			}
		}
	}

	// --- Bitwise functions ---

	void Not(bi_int& data) {

		for (std::size_t i = 0; i < data.Size; i++)
			data.Buffer[i] = ~data.Buffer[i];
	}

	void And(bi_int& first, const bi_int& second) {

		if (first.Size > second.Size) {

			for (std::size_t i = 0; i < second.Size; i++)
				first.Buffer[i] &= second.Buffer[i];

			for (std::size_t i = second.Size; i < first.Size; i++)
				first.Buffer[i] = 0;
		}

		else {

			for (std::size_t i = 0; i < first.Size; i++)
				first.Buffer[i] &= second.Buffer[i];
		}
	}

	void Or(bi_int& first, const bi_int& second) {

		if (first.Size > second.Size) {

			for (std::size_t i = 0; i < second.Size; i++)
				first.Buffer[i] |= second.Buffer[i];

			for (std::size_t i = second.Size; i < first.Size; i++)
				first.Buffer[i] = 0;
		}

		else {

			for (std::size_t i = 0; i < first.Size; i++)
				first.Buffer[i] |= second.Buffer[i];
		}
	}

	void Xor(bi_int& first, const bi_int& second) {

		if (first.Size > second.Size) {

			for (std::size_t i = 0; i < second.Size; i++)
				first.Buffer[i] ^= second.Buffer[i];

			for (std::size_t i = second.Size; i < first.Size; i++)
				first.Buffer[i] = 0;
		}

		else {

			for (std::size_t i = 0; i < first.Size; i++)
				first.Buffer[i] ^= second.Buffer[i];
		}
	}

	void ShiftLeft(bi_int& data, std::size_t bit_shift_amount) {

		std::size_t sizeInBytes = data.Size * sizeof(bi_type);
		if (bit_shift_amount >= sizeInBytes * 8) {

			memset(data.Buffer, 0, sizeInBytes);

			return;
		}

		std::uint8_t* buffer = (std::uint8_t*)data.Buffer;
		std::size_t offset = bit_shift_amount / 8;
		std::uint8_t rest = (std::uint8_t)(bit_shift_amount % 8);
		bi_memmove(buffer + offset, sizeInBytes, buffer, sizeInBytes - offset);
		memset(buffer, 0, offset);

		// Shift the last 'rest' bits to the left

		std::uint8_t* byte;
		for (byte = sizeInBytes - 1 + (std::uint8_t*)data.Buffer; sizeInBytes--; byte--) {

			std::uint8_t bits = 0;
			if (sizeInBytes)
				bits = byte[-1] & (0xFF << (CHAR_BIT - rest));

			*byte <<= rest;
			*byte |= (bits >> (CHAR_BIT - rest));
		}
	}

	void ShiftRight(bi_int& data, std::size_t bit_shift_amount, bool ext_sign) {

		std::size_t sizeInBytes = data.Size * sizeof(bi_type);
		const bi_type sign = BI_SIGN(data);
		if (bit_shift_amount >= sizeInBytes * 8) {

			memset(data.Buffer, ext_sign ? sign : 0, sizeInBytes);

			return;
		}

		std::uint8_t* buffer = (std::uint8_t*)data.Buffer;
		std::size_t offset = bit_shift_amount / 8;
		std::uint8_t rest = (std::uint8_t)(bit_shift_amount % 8);
		bi_memmove(buffer, sizeInBytes, buffer + offset, sizeInBytes - offset);
		memset(buffer + sizeInBytes - offset, ext_sign ? sign : 0, offset);

		// Shift the last 'rest' bits to the right

		std::uint8_t* byte;
		for (byte = (std::uint8_t*)data.Buffer; sizeInBytes--; byte++) {

			std::uint8_t bits = 0;
			if (sizeInBytes)
				bits = byte[1] & (0xFF >> (CHAR_BIT - rest));

			*byte >>= rest;
			*byte |= (bits << (CHAR_BIT - rest));
		}

		if (ext_sign)
			buffer[data.Size * sizeof(bi_type) - 1] |= (sign << (CHAR_BIT - rest));
	}

	// --- String functions ---

	std::string ToString(const bi_int& data) {

		// If the big integer is allocated on the stack, we can use the library functions to convert the number to string
		if (IsOnStack(data))
			return std::to_string((std::int64_t)GetSNO(data));

		// Shifts left by one bit
		void(*ShiftLeft1)(std::uint8_t* buffer, std::size_t size_in_bytes)
			=
			[](std::uint8_t* buffer, std::size_t size_in_bytes) {

				std::uint8_t* byte;
				for (byte = (std::uint8_t*)buffer; size_in_bytes--; byte++) {

					std::uint8_t bit = 0;
					if (size_in_bytes)
						bit = byte[1] & (1 << (CHAR_BIT - 1)) ? 1 : 0;

					*byte <<= 1;
					*byte |= bit;
				}
			};

		// Shifts left by 4 bits
		void(*ShiftLeft4)(std::uint8_t* buffer, std::size_t size_in_bytes)
			=
			[](std::uint8_t* buffer, std::size_t size_in_bytes) {

				std::uint8_t* byte;
				for (byte = (std::uint8_t*)buffer; size_in_bytes--; byte++) {

					std::uint8_t bit = 0;
					if (size_in_bytes)
						bit = (byte[1] & (0xFF << (CHAR_BIT - 4))) >> (CHAR_BIT - 4);

					*byte <<= 4;
					*byte |= bit;
				}
			};

		const bool isNegative = IsNegative(data);

		bi_int convertedData;
		Resize(convertedData, data.Size, false);
		Copy(convertedData, data);

		// Re-Convert from 2cp
		if (isNegative) {

			Decrement(convertedData);
			Not(convertedData);
		}

		// Big integer significant bits
		const std::size_t significantBits = CountSignificantBits(convertedData.Buffer, convertedData.Size);

		// Size in bytes = ceil(4*ceil(n/3)/8) + 1
		// The last one is needed as an auxiliary buffer to store the first 8 bits in the number
		const std::size_t bcdBufferSize = 2 * (std::size_t)ceil(ceil((long double)significantBits / 3.0) / 2.0) + 1;

		// The binary-coded decimal buffer
		std::uint8_t* bcdBuffer = (std::uint8_t*)calloc(bcdBufferSize, sizeof(std::uint8_t));
		if (bcdBuffer == NULL)
			return "0";

		std::uint8_t* buffer = (std::uint8_t*)convertedData.Buffer; // Big integer buffer as byte array
		const std::size_t bufferSize = ceil((long double)significantBits / 8.0); // Big integer buffer size as byte array
		const std::size_t shiftAmount = bufferSize * 8; // The amount of shifts needed to convert from binary representation to binary-coded decimal representation
		const std::size_t AUX_BUFFER_INDEX = bcdBufferSize - 1; // The index of the auxiliary buffer in the bcdBuffer (the last buffer's cell)
		for (std::size_t shift = 0; shift < shiftAmount; shift++) {

			// If the shift counter consumes all the bits in the auxiliar buffer, refill it with new data from the big integer buffer
			if (shift % 8 == 0)
				bcdBuffer[AUX_BUFFER_INDEX] = buffer[bufferSize - 1 - shift / 8];

			// Check every 4 bits if there is at least 5. If so, add 3 to the 4 bits
			for (std::size_t i = AUX_BUFFER_INDEX / 2; i < AUX_BUFFER_INDEX; i++) { // - (shift / 8 + 1)

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
		for (std::size_t i = 0; i < AUX_BUFFER_INDEX; i++) {

			ShiftLeft4(bcdBuffer, AUX_BUFFER_INDEX - i);
			bcdBuffer[bcdBufferSize - 2 - i] >>= 4;
		}

		// Remove the useless bits at the start
		std::size_t offset = 0;
		for (; offset < bcdBufferSize - 1; offset++)
			if (bcdBuffer[offset] != 0)
				break;

		// If the number is zero
		if (offset == AUX_BUFFER_INDEX)
			return "0";

		// Create the actual digit string
		std::string digitStr;
		digitStr.resize(bcdBufferSize - 1 - offset + isNegative);
		bi_memcpy(digitStr.data() + isNegative, bcdBufferSize - 1 - offset + isNegative, bcdBuffer + offset, bcdBufferSize - 1 - offset);
		if (isNegative)
			digitStr[0] = '-';

		// Convert from binary number to the ASCII character number
		for (std::size_t i = isNegative; i < digitStr.size(); i++)
			digitStr[i] += 48;

		// Free the bcd buffer
		free(bcdBuffer);

		// Clear the converted data
		Clear(convertedData);

		return digitStr;
	}

	bool FromString(bi_int& data, const std::string& str) {

		return false;
	}
}
