#include <cmath>

#include "Utils.hpp"

#undef BI_SIGN
#define BI_SIGN(x) x.Buffer[x.Size - 1]

/// <summary>
/// Resizes the two given big integers to add padding bits to make the size a power of 2. Mainly used in the Karatsuba algorithm implementation
/// </summary>
/// <param name="first">The first big integer</param>
/// <param name="second">The second big integer</param>
/// <returns>The new size of both numbers</returns>
inline static std::size_t MakeSameSizeAsPowerOf2(bi_int& first, bi_int& second) {

	const std::size_t maxBitSize = std::max(Utils::CountSignificantBits(first.Buffer, first.Size), Utils::CountSignificantBits(second.Buffer, second.Size));
	const std::size_t newSize = (std::size_t)std::ceill(pow(2.0l, (long long)log2(maxBitSize) + 1) / 8.0l);

	Utils::Resize(first, newSize);
	Utils::Resize(second, newSize);

	return newSize;
}

/// <summary>
/// Calculates the minimum byte size for an integer
/// </summary>
/// <param name="n">The desired integer</param>
/// <returns>The minimum byte size for the specified integer</returns>
inline static std::size_t CalcMinByteSize(const std::uint64_t& n) {

	return n == 0 ? 1 : (std::size_t)std::ceill((long double)((std::size_t)log2l((long double)n) + 1) / 8.0l);
}

/// <summary>
/// Calculates the minimum byte size for a big integer
/// </summary>
/// <param name="n">The desired big integer</param>
/// <returns>The minimum byte size for the specified big integer</returns>
inline static std::size_t CalcMinByteSize(const bi_int& n) {

	std::size_t bits = Utils::CountSignificantBits(n.Buffer, n.Size);

	return bits == 0 ? 1 : (std::size_t)std::ceill((long double)bits / 8.0l);
}

/// <summary>
/// Executes the Karatsuba algorithm to multiply two big integers of the same size
/// </summary>
/// <param name="x">The first factor (must be positive and without the sign byte)</param>
/// <param name="y">The second factor (must be positive and without the sign byte)</param>
/// <param name="size">Both factors size</param>
/// <returns>The product between the first and the second factor</returns>
inline static bi_int Karatsuba(const bi_type* const x, const bi_type* const y, std::size_t size) {

	if (size <= sizeof(std::uint32_t)) {

		const std::uint64_t product =
			Utils::BytesToQWORD(x, size) *
			Utils::BytesToQWORD(y, size);
		bi_int p;
		Utils::Resize(p, CalcMinByteSize(product) + 1, false);
		std::uint8_t* buffer = (std::uint8_t*)p.Buffer;
		bi_memcpy(buffer, p.Size * sizeof(bi_type), &product, CalcMinByteSize(product));

		return p;
	}

	// X1, X2
	bi_int x1, x2;
	Utils::Resize(x1, size / 2 + 1, false);
	Utils::Resize(x2, size / 2 + 1, false);
	Utils::Copy(x1, { (bi_type*)x + size / 2, size / 2 });
	Utils::Copy(x2, { (bi_type*)x, size / 2 });

	// Y1, Y2
	bi_int y1, y2;
	Utils::Resize(y1, size / 2 + 1, false);
	Utils::Resize(y2, size / 2 + 1, false);
	Utils::Copy(y1, { (bi_type*)y + size / 2, size / 2 });
	Utils::Copy(y2, { (bi_type*)y, size / 2 });

	// U
	bi_int u = Karatsuba(x1.Buffer, y1.Buffer, (x1.Size - 1));

	// V
	bi_int v = Karatsuba(x2.Buffer, y2.Buffer, (x2.Size - 1));

	// X1 - X2
	Utils::Negate(x2);
	Utils::Add(x1, x2);

	// Y1 - Y2
	Utils::Negate(y2);
	Utils::Add(y1, y2);

	// Save the original sign
	const bi_type x1Sign = BI_SIGN(x1);
	const bi_type y1Sign = BI_SIGN(y1);

	// Make the results positive
	Utils::Abs(x1);
	Utils::Abs(y1);

	// W = (X1 - X2) * (Y1 - Y2)
	bi_int w = Karatsuba(x1.Buffer, y1.Buffer, size / 2);

	// Add the sign
	if (x1Sign != y1Sign)
		Utils::Negate(w);

	bi_int z;
	Utils::Resize(z, u.Size, false);
	Utils::Copy(z, u);

	// Z = U + V - W
	Utils::Add(z, v);
	Utils::Negate(w);
	Utils::Add(z, w);

	const long double addSizeU = std::ceill(((long double)size * 8.0l - (long double)(u.Size * 8 - Utils::CountSignificantBits(u.Buffer, u.Size))) / 8.0l) + 1;
	const long double addSizeZ = std::ceill(((long double)(size / 2) * 8.0l - (long double)(z.Size * 8 - Utils::CountSignificantBits(z.Buffer, z.Size))) / 8.0l) + 1;

	// P = U * 2^n + Z * 2^n/2 + V
	Utils::Resize(u, u.Size + std::size_t((long double)u.Size + addSizeU), false);
	Utils::Resize(z, z.Size + std::size_t((long double)z.Size + addSizeZ), false);
	Utils::ShiftLeft(u.Buffer, u.Size * sizeof(bi_type), size * 8);
	Utils::ShiftLeft(z.Buffer, z.Size * sizeof(bi_type), size * 8 / 2);

	Utils::Add(u, z);
	Utils::Add(u, v);

	// Free memory
	Utils::Clear(x1);
	Utils::Clear(x2);
	Utils::Clear(y1);
	Utils::Clear(y2);
	Utils::Clear(v);
	Utils::Clear(w);
	Utils::Clear(z);

	return u;
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

	void Resize(bi_int& data, std::size_t new_size, bool ext_sign) {

		const std::size_t& old_size = data.Size;
		if (data.Buffer == nullptr) {

			PRINT("Resize called (data: nullptr, new_size: %zu)", new_size);
		}

		else {

			PRINT("Resize called (data: %p, old_size: %zu, new_size: %zu)", data.Buffer, old_size, new_size);
		}

		bi_type* tmp = data.Buffer;
		data.Buffer = new bi_type[new_size];
		memset(data.Buffer, 0, new_size * sizeof(bi_type)); // For some reason, this must not be touched

		if (tmp != nullptr) {

			// Copy the old data
			bi_memmove(data.Buffer, new_size * sizeof(bi_type), tmp, old_size > new_size ? new_size : old_size * sizeof(bi_type));

			// Fill the rest of the new buffer data with the old data sign
			if (ext_sign)
				for (std::size_t i = old_size; i < new_size; i++)
					data.Buffer[i] = tmp[old_size - 1];

			delete[] tmp;
		}

		data.Size = new_size;
	}

	void Copy(bi_int& dest, const bi_int& src, const std::size_t offset_dest) {

		bi_memcpy(dest.Buffer + offset_dest, dest.Size * sizeof(bi_type), src.Buffer, (src.Size) * sizeof(bi_type));
	}

	void Clear(bi_int& data) {

		if (data.Buffer != nullptr) {

			PRINT("Clear called (data: %p, size: %zu)", data.Buffer, data.Size);
			delete[] data.Buffer;
		}

		data.Buffer = nullptr;
		data.Size = 0;
	}

	void ShrinkToFit(bi_int& data) {

		const bi_type sign = BI_SIGN(data);

		std::size_t size = 0;
		for (std::size_t i = 0; i < data.Size; i++) {

			if (data.Buffer[data.Size - i - 1] != sign) {

				size = data.Size - i + 1;
				break;
			}
		}

		Resize(data, size, false);
	}

	std::uint64_t BytesToQWORD(const void* data, std::size_t size_in_bytes) {

		const std::uint8_t* buffer = (std::uint8_t*)data;
		std::uint64_t qword = 0;

		size_in_bytes = std::min(size_in_bytes, (std::size_t)8);
		for (std::size_t i = 0; i < size_in_bytes; i++)
			qword |= static_cast<std::uint64_t>(buffer[i]) << (i * 8);

		return qword;
	}

	bool IsNegative(const bi_int& data) {

		return data.Buffer != nullptr && data.Buffer[data.Size - 1] == BI_MINUS_SIGN;
	}

	std::size_t CountSignificantBits(const bi_type* const n, std::size_t size) {

		std::size_t bitSize = 0;
		const bi_type& sign = n[size - 1];

		for (std::size_t i = 1; i < size; i++) {

			if (n[size - 1 - i] != sign) {

				std::size_t min = n[size - 1 - i];
				bitSize = (size - 1 - i) * 8;
				bitSize += (std::size_t)log2(min) + 1;
				break;
			}
		}

		return bitSize;
	}

	// --- Mathematical operations ---

	int Compare(const bi_int& first, const bi_int& second) {

		const bi_type* firstByte = first.Buffer + first.Size - 1;
		const bi_type* secondByte = second.Buffer + second.Size - 1;

		// If the first one is positive and the second one is negative, then return 1, else return -1
		if (firstByte[0] < secondByte[0])
			return 1;
		else if (firstByte[0] > secondByte[0])
			return -1;

		const std::size_t firstBitSize = CountSignificantBits(first.Buffer, first.Size);
		const std::size_t secondBitSize = CountSignificantBits(second.Buffer, second.Size);

		// If the number of significant bits in the first number is higher than the second one, then return 1, else return -1
		if (firstBitSize > secondBitSize)
			return 1;
		else if (firstBitSize < secondBitSize)
			return -1;

		std::size_t size = (std::size_t)std::ceill((long double)firstBitSize / 8.0l);

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

		Not(data.Buffer, data.Size * sizeof(bi_type));
		Increment(data);
	}

	void Abs(bi_int& data) {

		if (IsNegative(data))
			Negate(data);
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

	void Mult(bi_int& first, const bi_int& second) {

		// First factor sign
		const bi_type sign1 = BI_SIGN(first);

		// Second factor sign
		const bi_type sign2 = BI_SIGN(second);

		bi_int x;
		Utils::Resize(x, first.Size, false);
		Utils::Copy(x, first);

		bi_int y;
		Utils::Resize(y, second.Size, false);
		Utils::Copy(y, second);

		Abs(x);
		Abs(y);

		std::size_t size = MakeSameSizeAsPowerOf2(x, y);

		Utils::Clear(first);
		first = Karatsuba(x.Buffer, y.Buffer, size);

		// Add the sign
		if (sign1 != sign2)
			Utils::Negate(first);

		ShrinkToFit(first);
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
		bi_memmove(buffer + offset, size_in_bytes, buffer, size_in_bytes - offset);
		memset(buffer, 0, offset);

		// Shift the last 'rest' bits to the left

		std::uint8_t* byte;
		for (byte = size_in_bytes - 1 + (std::uint8_t*)data; size_in_bytes--; byte--) {

			std::uint8_t bits = 0;
			if (size_in_bytes)
				bits = byte[-1] & (0xFF << (CHAR_BIT - rest));

			*byte <<= rest;
			*byte |= (bits >> (CHAR_BIT - rest));
		}
	}

	void ShiftRight(void* data, std::size_t size_in_bytes, std::size_t shift_amount) {

		if (shift_amount >= size_in_bytes * 8) {

			memset(data, 0, size_in_bytes);

			return;
		}

		std::uint8_t* buffer = (std::uint8_t*)data;
		std::size_t offset = shift_amount / 8;
		std::uint8_t rest = (std::uint8_t)(shift_amount % 8);
		bi_memmove(buffer, size_in_bytes, buffer + offset, size_in_bytes - offset);
		memset(buffer + size_in_bytes - offset, 0, offset);

		std::uint8_t* byte;
		for (byte = (std::uint8_t*)data; size_in_bytes--; byte++) {

			std::uint8_t bits = 0;
			if (size_in_bytes)
				bits = byte[1] & (0xFF >> (CHAR_BIT - rest));

			*byte >>= rest;
			*byte |= (bits << (CHAR_BIT - rest));
		}
	}

	void ShiftLeft1BE(void* data, std::size_t size_in_bytes) {

		std::uint8_t* byte;
		for (byte = (std::uint8_t*)data; size_in_bytes--; byte++) {

			std::uint8_t bit = 0;
			if (size_in_bytes)
				bit = byte[1] & (1 << (CHAR_BIT - 1)) ? 1 : 0;

			*byte <<= 1;
			*byte |= bit;
		}
	}

	void ShiftRight1BE(void* data, std::size_t size_in_bytes) {

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
