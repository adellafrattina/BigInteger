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

	void PrintAsBinary(void* data, std::size_t size_in_bytes) {

		std::uint8_t* buffer = (std::uint8_t*)data;

		for (std::size_t i = 0; i < size_in_bytes; i++) {

			std::uint8_t mask;
			for (mask = 1 << (CHAR_BIT - 1); mask; mask >>= 1)
				putchar(mask & buffer[size_in_bytes - 1 - i] ? '1' : '0');
		}

		putchar('\n');
	}

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
			bi_memmove(data.Buffer, new_size * sizeof(bi_type), tmp, old_size * sizeof(bi_type));

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

			const bi_type sign = BI_SIGN(dest);
			for (std::size_t i = src.Size; i < dest.Size; i++)
				dest.Buffer[i] = sign;
		}
	}

	void Clear(bi_int& data) {

		if (data.Size == BI_WORD_SIZE)
			SetSNO(data, 0);

		else {

			delete[] data.Buffer;
			SetSNO(data, 0);
		}
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

	inline std::size_t GetBitSize(WORD n) {

		return std::size_t(log2(n)) + 1;
	}

	inline std::size_t GetByteSize(WORD n) {

		const std::size_t bits = std::size_t(log2(n)) + 1;

		return bits / 8 + (std::size_t)std::ceil((long double)bits / 8.0);
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
				bitSize += (std::size_t)log2(min) + 1;
				break;
			}
		}

		return bitSize == 0 ? 1 : bitSize;
	}

	// --- Mathematical functions ---

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
			if (value + 1 >= value)
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

		return std::string();
	}

	bool FromString(bi_int& data, const std::string& str) {

		return false;
	}
}
