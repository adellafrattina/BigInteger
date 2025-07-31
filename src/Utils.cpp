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

			const bi_type sign = dest.Buffer[src.Size - 1];
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
}
