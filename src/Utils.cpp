#include <memory>

#include "Utils.hpp"

#undef BI_SIGN
#define BI_SIGN(x) x.Buffer[x.Size - 1]

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

	void Resize(bi_int& data, std::size_t new_size, bool ext_sign) {

		const std::size_t& old_size = data.Size;
		PRINT("Resize called (data: %p, old_size: %zu, new_size: %zu)", data.Buffer, old_size, new_size);

		bi_type* tmp = data.Buffer;
		data.Buffer = new bi_type[new_size];
		memset(data.Buffer, 0, new_size * sizeof(bi_type)); // For some reason, this must not be touched

		if (tmp != nullptr) {

			// Copy the old data
			memmove_s(data.Buffer, new_size * sizeof(bi_type), tmp, old_size * sizeof(bi_type));

			// Fill the rest of the new buffer data with the old data sign
			if (ext_sign)
				for (std::size_t i = old_size; i < new_size; i++)
					data.Buffer[i] = tmp[old_size - 1];

			delete[] tmp;
		}

		data.Size = new_size;
	}

	std::uint64_t BytesToQWORD(const void* data, std::size_t size_in_bytes) {

		const std::uint8_t* buffer = (std::uint8_t*)data;
		std::uint64_t qword = 0;

		for (std::size_t i = 0; i < size_in_bytes - 1; i++)
			qword |= static_cast<std::uint64_t>(buffer[i] << (i * 8));

		return qword;
	}

	bool IsNegative(const bi_int& data) {

		return data.Buffer != nullptr && data.Buffer[data.Size - 1] == BI_MINUS_SIGN;
	}

	// --- Mathematical operations ---

	void Negate(bi_int& data) {

		Not(data.Buffer, data.Size * sizeof(bi_type));
		Increment(data);
	}

	void Increment(bi_int& data) {

		const bool sameSign = data.Buffer[data.Size - 1] == BI_PLUS_SIGN;

		std::uint8_t carry = 1;
		std::size_t i = 0;
		while (i < data.Size && carry != 0) {

			carry = 0;
			bi_type value = data.Buffer[i];
			data.Buffer[i]++;
			if (data.Buffer[i] <= value) {

				carry = 1;
				i++;
			}
		}

		if (sameSign) {

			if (data.Buffer[data.Size - 1] != 0) {

				Resize(data, data.Size, data.Size + 1);
				data.Buffer[data.Size] = 0;
				data.Size++;
			}
		}
	}

	void Decrement(bi_int& data) {

		const bool sameSign = data.Buffer[data.Size - 1] == BI_MINUS_SIGN;

		std::uint8_t carry = 1;
		std::size_t i = 0;
		while (i < data.Size && carry != 0) {

			carry = 0;
			bi_type value = data.Buffer[i];
			data.Buffer[i]--;
			if (data.Buffer[i] >= value) {

				carry = 1;
				i++;
			}
		}

		if (sameSign) {

			if (data.Buffer[data.Size - 1] != BI_MINUS_SIGN) {

				Resize(data, data.Size + 1);
				data.Buffer[data.Size] = BI_MINUS_SIGN;
				data.Size++;
			}
		}
	}

	void Add(bi_int& first, const bi_int& second) {

		// Check if the numbers have the same sign
		const bool sameSign = BI_SIGN(first) == BI_SIGN(second);

		// First addend sign
		const bi_type sign1 = BI_SIGN(first);

		// Second addend sign
		const bi_type sign2 = BI_SIGN(second);

		// Check which number is the biggest
		std::size_t size;
		if (first.Size > second.Size) {

			size = first.Size;
		}

		else if (first.Size < second.Size) {

			size = second.Size;
			Resize(first, size);
		}

		else {

			size = first.Size;
		}

		// Sum the numbers byte by byte (TODO: group in QWORDs and add multithreading)

		std::size_t i = 0;
		std::uint8_t carry = 0;
		while (i < size) {

			bi_type toSum = i >= second.Size ? sign2 : second.Buffer[i];

			bi_type value = first.Buffer[i];
			first.Buffer[i] = first.Buffer[i] + toSum;
			bool overflow = first.Buffer[i] < value;

			value = first.Buffer[i];
			first.Buffer[i] += carry;
			carry = overflow || first.Buffer[i] < value ? 1 : 0;

			i++;
		}

		// This is the part where the 2's complement gets fixed to work with infinite-precision integer arithmetic.
		// The logic behind this code is the following:

		// If the 2 big integers have the same sign at the beginning...
		if (carry && sameSign) {

			// ... and the sum result has a different sign from the previous one...
			if (first.Buffer[size - 1] != sign1) {

				// ... then resize the destination buffer and add the sign at the end
				Resize(first, size + 1, false);
				first.Buffer[size] = carry ? BI_MINUS_SIGN : BI_PLUS_SIGN;
				size++;
			}
		}
	}

	// --- Bitwise operations ---

	void Not(void* data, std::size_t size_in_bytes) {

		std::uint8_t* buffer = (std::uint8_t*)data;

		for (std::size_t i = 0; i < size_in_bytes; i++)
			buffer[i] = ~buffer[i];
	}

	void ShiftLeft(void* data, std::size_t size_in_bytes, std::size_t shift_amount) {

		if (shift_amount >= size_in_bytes * 8) {

			memset(data, 0, size_in_bytes);

			return;
		}

		std::uint8_t* buffer = (std::uint8_t*)data;
		std::size_t offset = shift_amount / 8;
		std::uint8_t rest = (std::uint8_t)(shift_amount % 8);
		memmove_s(buffer, size_in_bytes, buffer + offset, size_in_bytes - offset);
		memset(buffer + size_in_bytes - offset, 0, offset);

		// Shift the last 'rest' bits to the left

		std::uint8_t* byte;
		for (byte = (std::uint8_t*)data; size_in_bytes--; byte++) {

			std::uint8_t bit = 0;
			if (size_in_bytes)
				bit = byte[1] & (0xFF << (CHAR_BIT - rest));

			*byte <<= rest;
			*byte |= (bit >> (CHAR_BIT - rest));
		}

		//std::uint8_t bits1 = 0, bits2 = 0;
		//for (std::size_t i = 0; i < buffer_size - offset; i++) {

		//	bits2 = buffer[buffer_size - offset - i - 1] & (std::uint8_t)((std::uint8_t)0b11111111 << (std::uint8_t)(8 - rest));
		//	buffer[buffer_size - offset - i - 1] <<= rest;
		//	buffer[buffer_size - offset - i - 1] |= (bits1 >> (8 - rest));
		//	bits1 = bits2;
		//}
	}

	void ShiftRight(void* data, std::size_t size_in_bytes, std::size_t shift_amount) {

		if (shift_amount >= size_in_bytes * 8) {

			memset(data, 0, size_in_bytes);

			return;
		}

		std::uint8_t* buffer = (std::uint8_t*)data;
		std::size_t offset = shift_amount / 8;
		std::uint8_t rest = (std::uint8_t)(shift_amount % 8);
		memmove_s(buffer + offset, size_in_bytes, buffer, size_in_bytes - offset);
		memset(buffer, 0, offset);

		// Shift the last 'rest' bits to the right

		std::uint8_t* byte;
		for (byte = size_in_bytes - 1 + (std::uint8_t*)data; size_in_bytes--; byte--) {

			std::uint8_t bit = 0;
			if (size_in_bytes)
				bit = byte[-1] & (0xFF >> (CHAR_BIT - rest));

			*byte >>= rest;
			*byte |= (bit << (CHAR_BIT - rest));
		}

		//std::uint8_t bits1 = 0, bits2 = 0;
		//for (std::size_t i = 0; i < buffer_size - offset; i++) {

		//	bits2 = buffer[buffer_size - offset - i - 1] & (std::uint8_t)((std::uint8_t)0b11111111 << (std::uint8_t)(8 - rest));
		//	buffer[buffer_size - offset - i - 1] <<= rest;
		//	buffer[buffer_size - offset - i - 1] |= (bits1 >> (8 - rest));
		//	bits1 = bits2;
		//}
	}

	void ShiftLeft1(void* data, std::size_t size_in_bytes) {

		std::uint8_t* byte;
		for (byte = (std::uint8_t*)data; size_in_bytes--; byte++) {

			std::uint8_t bit = 0;
			if (size_in_bytes)
				bit = byte[1] & (1 << (CHAR_BIT - 1)) ? 1 : 0;

			*byte <<= 1;
			*byte |= bit;
		}
	}

	void ShiftRight1(void* data, std::size_t size_in_bytes) {

		std::uint8_t* byte;
		for (byte = size_in_bytes - 1 + (std::uint8_t*)data; size_in_bytes--; byte--) {

			std::uint8_t bit = 0;
			if (size_in_bytes)
				bit = byte[-1] & 1 ? 1 : 0;

			*byte >>= 1;
			*byte |= (bit << (CHAR_BIT - 1));
		}
	}
}
