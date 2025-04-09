#include <memory>

#include "Utils.hpp"

namespace Utils {

	void PrintAsBinary(void* data, std::size_t size_in_bytes) {

		std::uint8_t* buffer = (std::uint8_t*)data;

		for (std::size_t i = 0; i < size_in_bytes; i++) {

			std::uint8_t mask;
			for (mask = 1 << (CHAR_BIT - 1); mask; mask >>= 1)
				putchar(mask & buffer[size_in_bytes - 1 - i] ? '1' : '0');
		}
	}

	void Resize(bi_int*& data, std::size_t old_size, std::size_t new_size) {

		PRINT("Resize called (data: %p, old_size: %zu, new_size: %zu)", data, old_size, new_size);

		bi_int* tmp = data;
		data = new bi_int[new_size];
		memset(data, 0, new_size * sizeof(bi_int));

		if (tmp != nullptr) {

			memmove_s(data, new_size * sizeof(bi_int), tmp, old_size * sizeof(bi_int));
			delete[] tmp;
		}
	}

	// --- Mathematical operations ---

	std::size_t Increment(bi_int*& data, std::size_t size) {

		std::uint8_t carry = 1;

		std::size_t i = 0;
		while (i < size && carry != 0) {

			carry = 0;
			bi_int value = data[i];
			data[i] = data[i] + 1;
			if (data[i] <= value) {

				carry = 1;
				i++;
			}
		}

		if (carry) {

			Resize(data, size, size + 1);
			data[size] = 1;
			size++;
		}

		return size;
	}

	std::size_t Add(bi_int*& data_dest, std::size_t size_dest, bi_int* data_to_sum, std::size_t size_to_sum) {

		std::size_t size;
		if (size_dest > size_to_sum) {

			size = size_dest;
		}

		else if (size_dest < size_to_sum) {

			size = size_to_sum;
			Resize(data_dest, size_dest, size);
		}

		else {

			size = size_dest;
		}

		std::uint8_t carry = 0;

		std::size_t i = 0;
		while (i < size) {

			bi_int toSum = i >= size_to_sum ? 0 : data_to_sum[i];

			bi_int value = data_dest[i];
			data_dest[i] = data_dest[i] + toSum;
			bool overflow = data_dest[i] < value;
			data_dest[i] += carry;
			carry = overflow || data_dest[i] < value ? 1 : 0;

			i++;
		}

		if (carry) {

			Resize(data_dest, size, size + 1);
			data_dest[size] = 1;
			size++;
		}

		return size;
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
