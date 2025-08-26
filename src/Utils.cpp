#include <cmath>
#include <functional>

#include "Utils.hpp"

//#undef BI_WORD_SIZE
//#define BI_WORD_SIZE (sizeof(WORD) / sizeof(bi_type))

//#undef BI_SIGN_BUFFER
//#define BI_SIGN_BUFFER(b, s) ((b[s - 1] & ((bi_type)1 << (sizeof(bi_type) * 8 - 1))) ? BI_MINUS_SIGN : BI_PLUS_SIGN)
//#undef BI_SIGN
//#define BI_SIGN(x) BI_SIGN_BUFFER(x.Buffer, x.Size)
//
//WORD GetSNO(const bi_int& data) {
//
//	return Utils::BigIntegerToWORD(data);
//}
//
//void SetSNO(bi_int& data, WORD sno) {
//
//	data = Utils::BigIntegerFromWORD(sno);
//}
//
//void Reset(bi_int& data) {
//
//	data.m_SNO = 0;
//	data.Size = BI_WORD_SIZE;
//	data.Buffer = reinterpret_cast<bi_type*>(&data.m_SNO);
//}
//
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
//
//	 bool IsLittleEndian() {
//
//		std::uint16_t n = 1;
//		std::uint8_t* ptr = (std::uint8_t*)&n;
//
//		return ptr[0] == 1;
//	}

	bool IsOnStack(const bi_int& data) {

		return data.Size == 1;
	}

	void Resize(bi_int& data, std::size_t new_size) {

		const std::size_t old_size = data.Size;
		new_size = new_size <= 1 ? 1 : new_size;

		// There is no point in executing a resize if both sizes are equal
		if (old_size == new_size)
			return;

		PRINT("Resize called (data: %p, old_size: %zu, new_size: %zu)", data.Buffer, old_size, new_size);

		// This means that the big integer buffer was originally allocated on the heap, but now it will be allocated on the stack
		if (new_size == 1) {

			WORD* tmp = data.Buffer; // Data was allocated on the heap
			data = bi_int(*data.Buffer, data.Sign); //SetSNO(data, BigIntegerToWORD(data)); // Allocate on the stack
			delete[] tmp; // Free the heap memory
		}

		// Regardless of how the big integer buffer was allocated, now it will be allocated on the heap
		else {

			WORD* tmp = data.Buffer; // Previous allocated data
			data.Buffer = new WORD[new_size]; // Allocate on the heap
			memset(data.Buffer, 0, new_size * sizeof(WORD));

			// Copy the old data
			bi_memcpy(data.Buffer, new_size * sizeof(WORD), tmp, std::min(old_size, new_size) * sizeof(WORD));
			data.Size = new_size;

			// If the big integer buffer was allocated on the heap, delete the old memory
			if (old_size > 1)
				delete[] tmp;
		}
	}

	void Copy(bi_int& dest, const bi_int& src, const std::size_t offset_dest, const std::size_t offset_src) {

		bi_memcpy(dest.Buffer + offset_dest, dest.Size * sizeof(WORD), src.Buffer + offset_src, src.Size * sizeof(WORD));
	}

	void Move(bi_int& dest, bi_int& src) {

		if (IsOnStack(src)) {

			Copy(dest, src);
			src.m_SNO = 0;
		}

		else {

			dest.Buffer = src.Buffer;
			dest.Size = src.Size;

			delete[] src.Buffer;
			src.m_SNO = 0;
			src.Sign = BI_PLUS_SIGN;
			src.Size = 1;
		}
	}

	void Clear(bi_int& data) {

		if (data.Size != 1) {

			PRINT("Clear called (data: %p, size: %zu)", data.Buffer, data.Size);

			delete[] data.Buffer;

			data.Buffer = &data.m_SNO;
			data.Size = 1;
		}

		data.m_SNO = 0;
	}

	void ShrinkToFit(bi_int& data) {

		if (data.Size == 1)
			return;

		std::size_t size = 1;
		for (std::size_t i = 0; i < data.Size; i++) {

			if (data.Buffer[data.Size - 1 - i] != 0 || data.Size - i == 1) {

				size = data.Size + 1 - i;

				break;
			}
		}

		Resize(data, size);
	}

	std::size_t CountSignificantBits(const WORD* const data, std::size_t size) {

		std::size_t bitSize = 0;
		for (std::size_t i = 0; i < size; i++) {

			if (data[size - 1 - i] != 0) {

				const std::size_t index = size - 1 - i;
				WORD min = data[index];
				bitSize = index * sizeof(WORD) * 8 + 1;
				while (min >>= 1)
					++bitSize;
				break;
			}
		}

		return bitSize == 0 ? 1 : bitSize;
	}

	// --- Mathematical functions ---

	int Compare(const bi_int& first, const bi_int& second) {

		// The two numbers are identical
		constexpr int EQUAL = 0;

		// The first number is less than the second one
		constexpr int LESS = -1;

		// The first number is greater than the second one
		constexpr int GREATER = 1;

		// If the first one is positive and the second one is negative, then return 1, else return -1
		if (first.Sign < second.Sign)
			return GREATER;
		else if (first.Sign > second.Sign)
			return LESS;

		const std::size_t firstBitSize = CountSignificantBits(first.Buffer, first.Size);
		const std::size_t secondBitSize = CountSignificantBits(second.Buffer, second.Size);

		// If the number of significant bits in the first number is higher than the second one, then return 1, else return -1
		if (firstBitSize > secondBitSize)
			return GREATER;
		else if (firstBitSize < secondBitSize)
			return LESS;

		std::size_t size = std::min(first.Size, second.Size) - 1;

		// Check every byte in the two numbers
		do {

			if (first.Buffer[size] > second.Buffer[size])
				return GREATER;
			else if (first.Buffer[size] < second.Buffer[size])
				return LESS;

		} while (size--);

		return LESS;
	}

	void Negate(bi_int& data) {

		data.Sign = !data.Sign;
	}

	void Abs(bi_int& data) {

		if (data.Sign == BI_MINUS_SIGN)
			data.Sign = BI_PLUS_SIGN;
	}
//
//	void Increment(bi_int& data) {
//
//		const bool sameSign = BI_SIGN(data) == BI_PLUS_SIGN;
//		const std::size_t sizeAsWords = data.Size / sizeof(WORD);
//
//		std::uint8_t carry = 1;
//		std::size_t i = 0;
//		while (i < sizeAsWords && carry != 0) {
//
//			carry = 0;
//			WORD value = BytesToWORD((std::uint8_t*)data.Buffer + i * sizeof(WORD), sizeof(WORD));
//			BytesFromWORD((std::uint8_t*)data.Buffer + i * sizeof(WORD), sizeof(WORD), value + 1);
//			if (value + 1 <= value)
//				carry = 1;
//
//			i++;
//		}
//
//		i *= sizeof(WORD);
//		if (data.Size - i != 0 && carry != 0) {
//
//			WORD value = BytesToWORD((std::uint8_t*)data.Buffer + i, data.Size - i);
//			BytesFromWORD((std::uint8_t*)data.Buffer + i, data.Size - i, value + 1);
//		}
//
//		if (sameSign) {
//
//			if (BI_SIGN(data) != BI_PLUS_SIGN) {
//
//				Resize(data, data.Size + 1, false);
//				data.Buffer[data.Size - 1] = BI_PLUS_SIGN;
//			}
//		}
//	}
//
//	void Decrement(bi_int& data) {
//
//		const bool sameSign = BI_SIGN(data) == BI_MINUS_SIGN;
//		const std::size_t sizeAsWords = data.Size / sizeof(WORD);
//
//		std::uint8_t carry = 1;
//		std::size_t i = 0;
//		while (i < sizeAsWords && carry != 0) {
//
//			carry = 0;
//			WORD value = BytesToWORD((std::uint8_t*)data.Buffer + i * sizeof(WORD), sizeof(WORD));
//			BytesFromWORD((std::uint8_t*)data.Buffer + i * sizeof(WORD), sizeof(WORD), value - 1);
//			if (value - 1 >= value)
//				carry = 1;
//
//			i++;
//		}
//
//		i *= sizeof(WORD);
//		if (data.Size - i != 0 && carry != 0) {
//
//			WORD value = BytesToWORD((std::uint8_t*)data.Buffer + i, data.Size - i);
//			BytesFromWORD((std::uint8_t*)data.Buffer + i, data.Size - i, value - 1);
//		}
//
//		if (sameSign) {
//
//			if (BI_SIGN(data) != BI_MINUS_SIGN) {
//
//				Resize(data, data.Size + 1, false);
//				data.Buffer[data.Size - 1] = BI_MINUS_SIGN;
//			}
//		}
//	}
//
//	void Add(bi_int& first, const bi_int& second) {
//
//		// Set up the operands to be of the same size
//
//		bi_type* op1;
//		bi_type* op2;
//		bool shouldCleanup = false;
//		if (first.Size > second.Size) {
//
//			shouldCleanup = true;
//			op1 = first.Buffer;
//			op2 = new bi_type[first.Size];
//			bi_memcpy(op2, first.Size * sizeof(bi_type), second.Buffer, second.Size * sizeof(bi_type));
//			const bi_type sign = BI_SIGN(second);
//			for (std::size_t i = second.Size; i < first.Size; i++)
//				op2[i] = sign;
//		}
//
//		else if (first.Size < second.Size) {
//
//			op2 = second.Buffer;
//			Resize(first, second.Size);
//			op1 = first.Buffer;
//		}
//
//		else {
//
//			op1 = first.Buffer;
//			op2 = second.Buffer;
//		}
//
//		// The operands' size
//		const std::size_t size = first.Size;
//
//		// Check if the numbers have the same sign
//		const bool sameSign = BI_SIGN_BUFFER(op1, size) == BI_SIGN_BUFFER(op2, size);
//
//		// First addend sign
//		const bi_type op1OriginalSign = BI_SIGN_BUFFER(op1, size);
//
//		const std::size_t sizeAsWords = (size * sizeof(bi_type)) / sizeof(WORD);
//
//		std::uint8_t carry = 0;
//		std::size_t i = 0;
//		while (i < sizeAsWords) {
//
//			const WORD v1 = BytesToWORD((std::uint8_t*)op1 + i * sizeof(WORD), sizeof(WORD));
//			const WORD v2 = BytesToWORD((std::uint8_t*)op2 + i * sizeof(WORD), sizeof(WORD));
//			WORD sum = v1 + v2;
//			bool overflow = sum < v1;
//			BytesFromWORD(op1 + i * sizeof(WORD), sizeof(WORD), sum + carry);
//			carry = (overflow || sum + carry < sum) ? 1 : 0;
//
//			i++;
//		}
//
//		i *= sizeof(WORD);
//		if (size - i != 0) {
//
//			const WORD v1 = BytesToWORD(op1 + i, size - i);
//			const WORD v2 = BytesToWORD(op2 + i, size - i);
//			WORD sum = v1 + v2;
//			WORD value = sum;
//			sum += carry;
//			BytesFromWORD(op1 + i, size - i, sum);
//			if (IsLittleEndian())
//				carry = ((std::uint8_t*)&sum)[size - i] ? 1 : 0;
//			else
//				carry = ((std::uint8_t*)&sum)[sizeof(WORD) - (size - i)] ? 1 : 0;
//		}
//
//		// This is the part where the 2's complement gets fixed to work with infinite-precision integer arithmetic.
//		// The logic behind this code is the following:
//
//		// If the 2 big integers had the same sign at the beginning...
//		if (sameSign) {
//
//			// ... and the sum result has a different sign from the previous one...
//			if (BI_SIGN_BUFFER(op1, size) != op1OriginalSign) {
//
//				// ... then resize the destination buffer and add the sign at the end
//				Resize(first, size + 1, false);
//				first.Buffer[size] = carry ? BI_MINUS_SIGN : BI_PLUS_SIGN;
//			}
//		}
//
//		if (shouldCleanup)
//			delete[] op2;
//	}
//
//	void Mult(bi_int& first, const bi_int& second) {
//
//		auto CountSignificantBitsU
//		=
//		[](const bi_type* const data, const std::size_t size) {
//
//			std::size_t bitSize = 0;
//			for (std::size_t i = 0; i < size; i++) {
//
//				if (data[size - 1 - i] != 0) {
//
//					const std::size_t index = size - 1 - i;
//					bi_type min = data[index];
//					bitSize = index * 8 + 1;
//					while (min >>= 1)
//						++bitSize;
//					break;
//				}
//			}
//
//			return bitSize == 0 ? 1 : bitSize;
//		};
//
//		std::function<void(bi_int&, const bi_int&)> Karatsuba;
//		Karatsuba
//		=
//		[&](bi_int& a, const bi_int& b) {
//
//			const std::size_t aBitSize = CountSignificantBitsU(a.Buffer, a.Size);
//			const std::size_t bBitSize = CountSignificantBitsU(b.Buffer, b.Size);
//			const std::size_t aSize = std::max((std::size_t)std::ceil((long double)aBitSize / 8.0), sizeof(std::uint32_t));
//			const std::size_t bSize = std::max((std::size_t)std::ceil((long double)bBitSize / 8.0), sizeof(std::uint32_t));
//
//			if (std::max(aSize, bSize) <= sizeof(std::uint32_t)) {
//
//				WORD res = BytesToWORD(a.Buffer, aSize) * BytesToWORD(b.Buffer, bSize);
//				memset(a.Buffer, 0, a.Size);
//				BytesFromWORD(a.Buffer, a.Size, res);
//			}
//
//			else {
//
//				// Unsigned sum. This function will not increase the big integers' size in case of overflow
//				auto AddU
//				=
//				[](bi_type* first, std::size_t first_size, const bi_type* second, std::size_t second_size) {
//
//					bi_type* const op1 = first;
//					const bi_type* const op2 = second;
//
//					// The operand's minimum size
//					const std::size_t minSize = std::min(first_size, second_size);
//
//					// The operand's size as words
//					const std::size_t sizeAsWords = (minSize * sizeof(bi_type)) / sizeof(WORD);
//
//					std::uint8_t carry = 0;
//					std::size_t i = 0;
//					while (i < sizeAsWords) {
//
//						const WORD v1 = BytesToWORD((std::uint8_t*)op1 + i * sizeof(WORD), sizeof(WORD));
//						const WORD v2 = BytesToWORD((std::uint8_t*)op2 + i * sizeof(WORD), sizeof(WORD));
//						const WORD sum = v1 + v2;
//						const bool overflow = sum < v1;
//						BytesFromWORD((std::uint8_t*)op1 + i * sizeof(WORD), sizeof(WORD), sum + carry);
//						carry = (overflow || sum + carry < sum) ? 1 : 0;
//
//						i++;
//					}
//
//					i *= sizeof(WORD);
//					while (i < first_size) {
//
//						const WORD v1 = BytesToWORD((std::uint8_t*)op1 + i, first_size - i);
//						const WORD v2 = i < second_size ? BytesToWORD((std::uint8_t*)op2 + i, second_size - i) : 0;
//						const WORD sum = v1 + v2;
//						const bool overflow = sum < v1;
//						BytesFromWORD((std::uint8_t*)op1 + i, first_size - i, sum + carry);
//						carry = (overflow || sum + carry < sum) ? 1 : 0;
//
//						i += first_size - i > sizeof(WORD) ? sizeof(WORD) : first_size - i;
//					}
//				};
//
//				// Unsigned subtraction (first must be greater or equal to second). This function will not increase the big integers' size in case of overflow
//				auto SubU
//				=
//				[](bi_type* first, std::size_t first_size, const bi_type* second, std::size_t second_size) {
//
//					short borrow = 0;
//					for (std::size_t i = 0; i < first_size; i++) {
//
//						short f = first[i];
//						short s = i < second_size ? second[i] : 0;
//
//						short diff = f - s - borrow;
//
//						if (diff < 0) {
//
//							diff += 256;
//							borrow = 1;
//						}
//
//						else
//							borrow = 0;
//
//						first[i] = (std::uint8_t)diff;
//					}
//				};
//
//				// Split
//
//				const std::size_t size = std::max(aSize, bSize);
//				const std::size_t sp = size / 2;
//
//				// A0
//				bi_int a0;
//				Utils::Resize(a0, sp, false);
//				for (std::size_t i = 0; i < sp; i++)
//					a0.Buffer[i] = i < aSize ? a.Buffer[i] : 0;
//
//				// A1
//				bi_int a1;
//				Utils::Resize(a1, sp + 2, false); // Plus 2 for the possible carry in the later sum
//				for (std::size_t i = sp; i < size; i++)
//					a1.Buffer[i - sp] = i < aSize ? a.Buffer[i] : 0;
//
//				// B0
//				bi_int b0;
//				Utils::Resize(b0, sp, false);
//				for (std::size_t i = 0; i < sp; i++)
//					b0.Buffer[i] = i < bSize ? b.Buffer[i] : 0;
//
//				// B1
//				bi_int b1;
//				Utils::Resize(b1, sp + 2, false); // Plus 2 for the possible carry in the later sum
//				for (std::size_t i = sp; i < size; i++)
//					b1.Buffer[i - sp] = i < bSize ? b.Buffer[i] : 0;
//
//				// K1
//				bi_int k1;
//				std::size_t a1Size = (std::size_t)std::ceil((long double)CountSignificantBitsU(a1.Buffer, a1.Size) / 8.0);
//				Utils::Resize(k1, a1Size + (std::size_t)std::ceil((long double)CountSignificantBitsU(b1.Buffer, b1.Size) / 8.0) + 2 * sp, false);
//				bi_memcpy(k1.Buffer, k1.Size * sizeof(bi_type), a1.Buffer, a1Size * sizeof(bi_type));
//				Karatsuba(k1, b1);
//
//				// K2
//				bi_int k2;
//				AddU(a1.Buffer, a1.Size, a0.Buffer, a0.Size);
//				AddU(b1.Buffer, b1.Size, b0.Buffer, b0.Size);
//				a1Size = (std::size_t)std::ceil((long double)CountSignificantBitsU(a1.Buffer, a1.Size) / 8.0);
//				Utils::Resize(k2, a1Size + (std::size_t)std::ceil((long double)CountSignificantBitsU(b1.Buffer, b1.Size) / 8.0) + sp, false); //(std::size_t)std::ceil((long double)(CountSignificantBitsU(a1.Buffer, a1.Size) + CountSignificantBitsU(b1.Buffer, b1.Size)) / 8.0)
//				bi_memcpy(k2.Buffer, k2.Size * sizeof(bi_type), a1.Buffer, a1Size * sizeof(bi_type));
//				Karatsuba(k2, b1);
//
//				// K3
//				bi_int k3;
//				std::size_t a0Size = (std::size_t)std::ceil((long double)CountSignificantBitsU(a0.Buffer, a0.Size) / 8.0);
//				Utils::Resize(k3, a0Size + (std::size_t)std::ceil((long double)CountSignificantBitsU(b0.Buffer, b0.Size) / 8.0), false); //(std::size_t)std::ceil((long double)(CountSignificantBitsU(a0.Buffer, a0.Size) + CountSignificantBitsU(b0.Buffer, b0.Size)) / 8.0)
//				bi_memcpy(k3.Buffer, k3.Size * sizeof(bi_type), a0.Buffer, a0Size * sizeof(bi_type));
//				Karatsuba(k3, b0);
//
//				SubU(k2.Buffer, k2.Size, k3.Buffer, k3.Size);
//				SubU(k2.Buffer, k2.Size, k1.Buffer, k1.Size);
//
//				Utils::ShiftLeft(k1, 16 * sp);
//				Utils::ShiftLeft(k2, 8 * sp);
//
//				memset(a.Buffer, 0, a.Size);
//				AddU(a.Buffer, a.Size, k1.Buffer, k1.Size);
//				AddU(a.Buffer, a.Size, k2.Buffer, k2.Size);
//				AddU(a.Buffer, a.Size, k3.Buffer, k3.Size);
//			}
//		};
//
//		std::size_t size = std::max(first.Size, second.Size);
//
//		// Op1
//		bi_int& op1 = first;
//		//Utils::Resize(first, first.Size);
//		const bi_type op1Sign = BI_SIGN(first);
//		Utils::Abs(first);
//
//		// Op2
//		bi_int op2;
//		Utils::Resize(op2, second.Size, false);
//		Utils::Copy(op2, second, 0, false);
//		const bi_type op2Sign = BI_SIGN(op2);
//		Utils::Abs(op2);
//
//		const std::size_t firstBitSize = CountSignificantBitsU(op1.Buffer, op1.Size);
//		const std::size_t secondBitSize = CountSignificantBitsU(op2.Buffer, op2.Size);
//		const std::size_t resultSize = std::max(size, (std::size_t)std::ceil((long double)(firstBitSize + secondBitSize + 1) / 8.0));
//
//		// Karatsuba algorithm
//		Utils::Resize(op1, resultSize);
//		Karatsuba(op1, op2);
//
//		// Sign
//		if (op1Sign ^ op2Sign)
//			Negate(op1);
//	}

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

		if (bit_shift_amount > data.Size * sizeof(WORD) * 8) {

			memset(data.Buffer, 0, data.Size * sizeof(WORD));

			return;
		}

		WORD*& buffer = data.Buffer;
		std::size_t offset = bit_shift_amount / (sizeof(WORD) * 8);
		std::size_t rest = bit_shift_amount % (sizeof(WORD) * 8);
		bi_memmove(buffer + offset, data.Size * sizeof(WORD), buffer, data.Size * sizeof(WORD) - offset);
		memset(buffer, 0, offset * sizeof(WORD));

		WORD* word;
		std::size_t size = data.Size;
		for (word = data.Size - 1 + buffer; size--; word--) {

			WORD bits = 0;
			if (size)
				bits = word[-1] & (UINT64_MAX << (sizeof(WORD) * 8 - rest));

			*word <<= rest;
			*word |= (bits >> (sizeof(WORD) * 8 - rest));
		}
	}

	void ShiftRight(bi_int& data, std::size_t bit_shift_amount) {

		if (bit_shift_amount >= data.Size * sizeof(WORD) * 8) {

			memset(data.Buffer, 0, data.Size * sizeof(WORD));

			return;
		}

		WORD*& buffer = data.Buffer;
		std::size_t offset = bit_shift_amount / (sizeof(WORD) * 8);
		std::size_t rest = bit_shift_amount % (sizeof(WORD) * 8);
		bi_memmove(buffer, data.Size * sizeof(WORD), buffer + offset, data.Size * sizeof(WORD) - offset);
		memset(buffer + data.Size - offset, 0, offset * sizeof(WORD));

		// Shift the last 'rest' bits to the right

		WORD* word;
		std::size_t size = data.Size;
		for (word = data.Buffer; size--; word++) {

			WORD bits = 0;
			if (size)
				bits = word[1] & (UINT64_MAX >> (sizeof(WORD) * 8 - rest));

			*word >>= rest;
			*word |= (bits << (sizeof(WORD) * 8 - rest));
		}
	}

	// --- String functions ---

	std::string ToString(const bi_int& data) {

		// If the big integer is allocated on the stack, we can use the library functions to convert the number to string
		if (IsOnStack(data))
			return std::to_string(data.Sign ? -(std::int64_t)data.m_SNO : (std::int64_t)data.m_SNO);

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

		// Big integer significant bits
		const std::size_t significantBits = CountSignificantBits(data.Buffer, data.Size);

		// Size in bytes = ceil(4*ceil(n/3)/8) + 1
		// The last one is needed as an auxiliary buffer to store the first 8 bits in the number
		const std::size_t bcdBufferSize = 2 * (std::size_t)ceil(ceil((long double)significantBits / 3.0) / 2.0) + 1;

		// The binary-coded decimal buffer
		std::uint8_t* bcdBuffer = (std::uint8_t*)calloc(bcdBufferSize, sizeof(std::uint8_t));
		if (bcdBuffer == NULL)
			return "0";

		std::uint8_t* buffer = (std::uint8_t*)data.Buffer; // Big integer buffer as byte array
		const std::size_t bufferSize = (std::size_t)ceil((long double)significantBits / 8.0); // Big integer buffer size as byte array
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
		digitStr.resize(bcdBufferSize - 1 - offset + data.Sign);
		bi_memcpy(digitStr.data() + data.Sign, bcdBufferSize - 1 - offset + data.Sign, bcdBuffer + offset, bcdBufferSize - 1 - offset);
		if (data.Sign)
			digitStr[0] = '-';

		// Convert from binary number to the ASCII character number
		for (std::size_t i = data.Sign; i < digitStr.size(); i++)
			digitStr[i] += 48;

		// Free the bcd buffer
		free(bcdBuffer);

		return digitStr;
	}

	bool FromString(bi_int& data, const std::string& str) {

		// Shifts right by one bit
		void(*ShiftRight1)(std::uint8_t* buffer, std::size_t size_in_bytes)
			=
			[](std::uint8_t* buffer, std::size_t size_in_bytes) {

				std::uint8_t* byte;
				for (byte = size_in_bytes - 1 + (std::uint8_t*)buffer; size_in_bytes--; byte--) {

					std::uint8_t bit = 0;
					if (size_in_bytes)
						bit = byte[-1] & 1 ? 1 : 0;

					*byte >>= 1;
					*byte |= (bit << (CHAR_BIT - 1));
				}
			};

		// If the string is empty, return null
		if (str.empty())
			return false;

		// Check if the number is positive or negative
		data.Sign = str.at(0) == '-';
		const std::size_t strLength = data.Sign ? str.length() - 1 : str.length();
		if (strLength == 0)
			return false;

		// Check the string is made up by numbers only
		for (std::size_t i = data.Sign; i < str.length(); i++)
			if (str.at(i) < '0' || str.at(i) > '9')
				return false;

		// Reverse double dabble algorithm

		// Size in bytes = ceil(string length / 2) + 1
		// The last one is needed as an auxiliary buffer to store the first 8 bits in the final number
		const std::size_t bcdBufferSize = (std::size_t)(std::ceil((long double)strLength / 2.0)) + 1;

		// The binary-coded decimal buffer
		std::uint8_t* bcdBuffer = (std::uint8_t*)calloc(bcdBufferSize, sizeof(std::uint8_t));
		if (bcdBuffer == NULL)
			return "0";

		// Fill the bcd buffer with the provided data.
		// It starts at the end to ensure the last bits are adjacent to the auxiliary buffer,
		// so the algorithm can right shift them in the correct position

		long double i = 0.0;
		std::size_t strIndex = 0;
		std::uint8_t nibble_offset = LOW_BITS;
		std::uint8_t shiftAmount = 0;
		while (strIndex < strLength) {

			bcdBuffer[bcdBufferSize - 2 - (std::size_t)i] |= ((std::uint8_t)(str.at(strLength - 1 - strIndex + data.Sign) - '0') << shiftAmount) & nibble_offset;
			nibble_offset = ~nibble_offset;
			shiftAmount = shiftAmount == 4 ? 0 : 4;
			i += 0.5;
			strIndex++;
		}

		// Set up data

		const std::size_t dataSize = (std::size_t)std::ceil(std::ceil((long double)strLength * log2(10.0l) + 1) / (sizeof(WORD) * 8.0l));

		if (data.Size < dataSize) {

			Clear(data);
			Resize(data, dataSize);
		}

		else
			memset(data.Buffer, 0, data.Size * sizeof(WORD));

		std::size_t offset = 0;
		std::uint8_t* buffer = (std::uint8_t*)data.Buffer;
		for (std::size_t shift = 0; shift <= dataSize * sizeof(WORD) * 8; shift++) { // '<=' because we need to shift the last bit into the auxiliary buffer

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
}
