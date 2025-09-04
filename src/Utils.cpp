#include <cmath>
#include <functional>

#include "Utils.hpp"

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

	void Resize(BigInt_T& data, std::size_t new_size) {

		const std::size_t old_size = data.Size;
		new_size = new_size <= 1 ? 1 : new_size;

		// There is no point in executing a resize if both sizes are equal
		if (old_size == new_size)
			return;

		PRINT("Resize called (data: %p, old_capacity: %zu, new_capacity: %zu)", data.Buffer, old_size, new_size);

		// This means that the big integer buffer was originally allocated on the heap, but now it will be allocated on the stack
		if (new_size == 1) {

			WORD* tmp = data.Buffer; // Data was allocated on the heap
			data = BigInt_T(*data.Buffer, data.Sign); //SetSNO(data, BigIntegerToWORD(data)); // Allocate on the stack
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

	void Copy(BigInt_T& dest, const BigInt_T& src, const std::size_t offset_dest, const std::size_t offset_src) {

		bi_memcpy(dest.Buffer + offset_dest, dest.Size * sizeof(WORD), src.Buffer + offset_src, CountSignificantWords(src) * sizeof(WORD));
		dest.Sign = src.Sign;
	}

	void Move(BigInt_T& dest, BigInt_T& src) {

		if (IsOnStack(src)) {

			Copy(dest, src);
			src.SNO = 0;
		}

		else {

			Utils::Clear(dest);

			dest.Buffer = src.Buffer;
			dest.Sign = src.Sign;
			dest.Size = src.Size;

			src.SNO = 0;
			src.Sign = BI_PLUS_SIGN;
			src.Buffer = &src.SNO;
			src.Size = 1;
		}
	}

	void Clear(BigInt_T& data) {

		if (data.Size != 1) {

			PRINT("Clear called (data: %p, size: %zu)", data.Buffer, data.Size);

			delete[] data.Buffer;

			data.Buffer = &data.SNO;
			data.Size = 1;
		}

		data.SNO = 0;
	}

	void ShrinkToFit(BigInt_T& data) {

		if (data.Size == 1)
			return;

		std::size_t size = 1;
		for (std::size_t i = 0; i < data.Size; i++) {

			if (data.Buffer[data.Size - 1 - i] != 0 || data.Size - i == 1) {

				size = data.Size - i;
				break;
			}
		}

		Resize(data, size);
	}

	std::size_t CountSignificantBits(const BigInt_T& data) {

		std::size_t bits = 0;
		for (std::size_t i = 0; i < data.Size; i++) {

			if (data.Buffer[data.Size - 1 - i] != 0) {

				const std::size_t index = data.Size - 1 - i;
				bits = index * sizeof(WORD) * 8;
				WORD wrd = data.Buffer[index];
				while (wrd) {

					++bits;
					wrd >>= 1;
				}

				return bits;
			}
		}

		return 1;
	}

	std::size_t CountSignificantWords(const BigInt_T& data) {

		if (data.Size == 1)
			return 1;

		for (std::size_t i = 0; i < data.Size; i++)
			if (data.Buffer[data.Size - 1 - i] != 0)
				return data.Size - i;

		return 1;
	}

	// --- Mathematical functions ---

	int Compare(const BigInt_T& a, const BigInt_T& b) {

		// The two numbers are identical
		constexpr int EQUAL = 0;

		// The first number is less than the second one
		constexpr int LESS = -1;

		// The first number is greater than the second one
		constexpr int GREATER = 1;

		// If the first one is positive and the second one is negative, then return 1, else return -1
		if (a.Sign < b.Sign)
			return GREATER;
		else if (a.Sign > b.Sign)
			return LESS;

		const std::size_t firstBitSize = CountSignificantBits(a);
		const std::size_t secondBitSize = CountSignificantBits(b);

		// If the number of significant bits in the first number is higher than the second one, then return 1, else return -1
		if (firstBitSize > secondBitSize)
			return GREATER;
		else if (firstBitSize < secondBitSize)
			return LESS;

		std::size_t size = std::min((std::size_t)std::ceil((long double)firstBitSize / (sizeof(WORD) * 8)), (std::size_t)std::ceil((long double)secondBitSize / (sizeof(WORD) * 8))) - 1;

		// Check every word in the two numbers
		do {

			if (a.Buffer[size] > b.Buffer[size])
				return GREATER;
			else if (a.Buffer[size] < b.Buffer[size])
				return LESS;

		} while (size--);

		return EQUAL;
	}

	int CompareU(const BigInt_T& a, const BigInt_T& b) {

		// The two numbers are identical
		constexpr int EQUAL = 0;

		// The first number is less than the second one
		constexpr int LESS = -1;

		// The first number is greater than the second one
		constexpr int GREATER = 1;

		const std::size_t firstBitSize = CountSignificantBits(a);
		const std::size_t secondBitSize = CountSignificantBits(b);

		// If the number of significant bits in the first number is higher than the second one, then return 1, else return -1
		if (firstBitSize > secondBitSize)
			return GREATER;
		else if (firstBitSize < secondBitSize)
			return LESS;

		std::size_t size = std::min((std::size_t)std::ceil((long double)firstBitSize / (sizeof(WORD) * 8)), (std::size_t)std::ceil((long double)secondBitSize / (sizeof(WORD) * 8))) - 1;

		// Check every word in the two numbers
		do {

			if (a.Buffer[size] > b.Buffer[size])
				return GREATER;
			else if (a.Buffer[size] < b.Buffer[size])
				return LESS;

		} while (size--);

		return EQUAL;
	}

	void Negate(BigInt_T& data) {

		data.Sign = !data.Sign;
	}

	void Abs(BigInt_T& data) {

		if (data.Sign == BI_MINUS_SIGN)
			data.Sign = BI_PLUS_SIGN;
	}

	void Increment(BigInt_T& data) {

		const std::size_t actualSize = CountSignificantWords(data);

		// The number has the same sign as the number 1
		if (data.Sign == BI_PLUS_SIGN) {

			WORD carry = 1;
			for (std::size_t i = 0; i < actualSize && carry == 1; i++) {

				carry = 0;
				const WORD v = data.Buffer[i];
				++data.Buffer[i];
				if (data.Buffer[i] < v)
					carry = 1;
			}

			if (carry) {

				if (data.Size <= actualSize)
					Resize(data, actualSize + 1);
				data.Buffer[actualSize] = 1;
			}
		}

		else {

			if (actualSize == 1 && *data.Buffer == 1) {

				*data.Buffer = 0;
				data.Sign = BI_PLUS_SIGN;
			}

			else {

				WORD carry = 1;
				for (std::size_t i = 0; i < actualSize && carry == 1; i++) {

					carry = 0;
					const WORD v = data.Buffer[i];
					--data.Buffer[i];
					if (data.Buffer[i] > v)
						carry = 1;
				}
			}
		}
	}

	void Decrement(BigInt_T& data) {

		const std::size_t actualSize = CountSignificantWords(data);

		// The number has the same sign as the number -1
		if (data.Sign == BI_PLUS_SIGN) {

			if (actualSize && *data.Buffer == 0) {

				*data.Buffer = 1;
				data.Sign = BI_MINUS_SIGN;
			}

			else {

				WORD carry = 1;
				for (std::size_t i = 0; i < actualSize && carry == 1; i++) {

					carry = 0;
					const WORD v = data.Buffer[i];
					--data.Buffer[i];
					if (data.Buffer[i] > v)
						carry = 1;
				}
			}
		}

		else {

			WORD carry = 1;
			for (std::size_t i = 0; i < actualSize && carry == 1; i++) {

				carry = 0;
				const WORD v = data.Buffer[i];
				++data.Buffer[i];
				if (data.Buffer[i] < v)
					carry = 1;

				i++;
			}

			if (carry) {

				if (data.Size <= actualSize)
					Resize(data, actualSize + 1);
				data.Buffer[actualSize] = 1;
			}
		}
	}

	void Add(BigInt_T& a, const BigInt_T& b) {

		if (a.Sign == b.Sign)
			AddU(a, b);

		else {

			int cmp = CompareU(a, b);

			if (cmp > 0) {

				SubU(a, b);
			}

			else if (cmp < 0) {

				BigInt_T c = b;
				SubU(c, a);
				if (c.Size >= a.Size)
					Move(a, c);
				else
					Copy(a, c);
			}

			else {

				memset(a.Buffer, 0, a.Size * sizeof(WORD));
			}
		}
	}

	void AddU(BigInt_T& a, const BigInt_T& b) {

		std::size_t size = std::max(CountSignificantWords(a), CountSignificantWords(b));
		if (a.Size < size)
			Resize(a, size);

		WORD carry = 0;
		for (std::size_t i = 0; i < size; i++) {

			const WORD sum = a.Buffer[i] + (i < b.Size ? b.Buffer[i] : 0);
			const bool overflow = sum < a.Buffer[i];
			a.Buffer[i] = sum + carry;
			carry = (overflow || a.Buffer[i] < sum) ? 1 : 0;
		}

		if (carry != 0) {

			if (size < a.Size)
				a.Buffer[size] = 1;

			else {

				Resize(a, size + 1);
				a.Buffer[a.Size - 1] = 1;
			}
		}
	}

	void Sub(BigInt_T& a, const BigInt_T& b) {

		if (a.Sign != b.Sign)
			AddU(a, b);

		else {

			int cmp = CompareU(a, b);

			if (cmp > 0) {

				SubU(a, b);
			}

			else if (cmp < 0) {

				BigInt_T c = b;
				SubU(c, a);
				if (c.Size >= a.Size)
					Move(a, c);
				else
					Copy(a, c);
				a.Sign = BI_MINUS_SIGN;
			}

			else {

				memset(a.Buffer, 0, a.Size * sizeof(WORD));
			}
		}
	}

	void SubU(BigInt_T& a, const BigInt_T& b) {

		// @TODO: ASSERT THAT a.Size > b.Size and Compare(a, b) > 1 || Compare(a, b) == 0

		WORD borrow = 0;
		for (std::size_t i = 0; i < a.Size; i++) {

			const WORD& ai = a.Buffer[i];
			const WORD& bi = i < b.Size ? b.Buffer[i] : 0;

			WORD temp = ai - bi - borrow;
			borrow = (ai < bi + borrow) || (bi + borrow < bi);
			a.Buffer[i] = temp;
		}
	}

	void Mult(BigInt_T& first, const BigInt_T& second) {

		std::size_t firstSize = CountSignificantWords(first);
		std::size_t secondSize = CountSignificantWords(second);

		if (firstSize == 1 && secondSize == 1) {

			// If the number is less than BI_MAX_HALFWORD, we can simply multiply the two buffers
			if (*first.Buffer <= BI_MAX_HALFWORD &&
				*second.Buffer <= BI_MAX_HALFWORD)

			{

				*first.Buffer *= *second.Buffer;

				return;
			}
		}

		// Multiply two word sized numbers and store the result in a dobule word sized number
		static std::function<void(WORD, WORD, WORD&, WORD&)> MultU128
		=
		[](WORD a, WORD b, WORD& low, WORD& high) {

			/*

				Applied formula:

				(a_high × 2^32 + a_low) × (b_high × 2^32 + b_low)
				= a_high×b_high×2^64 + (a_high×b_low + a_low×b_high)×2^32 + a_low×b_low

			*/

			constexpr HALFWORD HALF_WORD_BITS = sizeof(WORD) * 4;

			// Split
			WORD aLow = a & BI_MAX_HALFWORD; //0xFFFFFFFFULL
			WORD aHigh = a >> HALF_WORD_BITS;
			WORD bLow = b & BI_MAX_HALFWORD;
			WORD bHigh = b >> HALF_WORD_BITS;

			WORD ll = aLow * bLow;
			WORD lh = aLow * bHigh;
			WORD hl = aHigh * bLow;
			WORD hh = aHigh * bHigh;

			WORD middle1 = (ll >> HALF_WORD_BITS) + (lh & BI_MAX_HALFWORD) + (hl & BI_MAX_HALFWORD);
			WORD middle2 = (lh >> HALF_WORD_BITS) + (hl >> HALF_WORD_BITS) + (middle1 >> HALF_WORD_BITS);

			low = (ll & BI_MAX_HALFWORD) | (middle1 << HALF_WORD_BITS);
			high = hh + middle2;
		};

		// Multiply by word
		static std::function<void(BigInt_T&, WORD)> MultiplyByWord
		=
		[](BigInt_T& a, WORD c) {

			WORD carry = 0;
			for (std::size_t i = 0; i < a.Size; i++) {

				WORD low, high;
				MultU128(a.Buffer[i], c, low, high);

				WORD temp = low + carry;
				if (temp < low)
					++high;

				a.Buffer[i] = temp;
				carry = high;
			}
		};

		// Basecase multiplication
		static std::function<void(BigInt_T&, const BigInt_T&)> Basecase
		=
		[](BigInt_T& a, const BigInt_T& b) {

			const std::size_t aSize = CountSignificantWords(a);
			const std::size_t bSize = CountSignificantWords(b);

			if (aSize == 1 && bSize == 1) {

				// If the number is less than 4294967295, we can simply multiply the two buffers
				if (*a.Buffer <= BI_MAX_HALFWORD &&
					*b.Buffer <= BI_MAX_HALFWORD)

				{

					*a.Buffer *= *b.Buffer;
				}

				else {

					MultU128(a.Buffer[0], b.Buffer[0], a.Buffer[0], a.Buffer[1]);
				}

				return;
			}

			BigInt_T c;
			Resize(c, aSize);
			Copy(c, a);
			MultiplyByWord(a, b.Buffer[0]);
			for (std::size_t j = 1; j < bSize; j++) {

				BigInt_T d;
				Resize(d, aSize + 1 + j);
				Copy(d, c);
				MultiplyByWord(d, b.Buffer[j]);
				ShiftLeft(d, j * sizeof(WORD) * 8);
				AddU(a, d);
			}
		};

		// Karatsuba algorithm
		static std::function<void(BigInt_T&, const BigInt_T&)> Karatsuba
		=
		[](BigInt_T& a, const BigInt_T& b) {

			std::size_t aSize = CountSignificantWords(a);
			std::size_t bSize = CountSignificantWords(b);

			if (aSize < 1024 || bSize < 1024) {

				Basecase(a, b);

				return;
			}

			// Split

			const std::size_t size = std::max(aSize, bSize);
			const std::size_t sp = size / 2;

			// A0
			BigInt_T a0;
			Resize(a0, sp);
			for (std::size_t i = 0; i < sp; i++)
				a0.Buffer[i] = i < aSize ? a.Buffer[i] : 0;

			// A1
			BigInt_T a1;
			Resize(a1, sp + 2); // Plus 2 for the possible carry in the later sum
			for (std::size_t i = sp; i < size; i++)
				a1.Buffer[i - sp] = i < aSize ? a.Buffer[i] : 0;

			// B0
			BigInt_T b0;
			Resize(b0, sp);
			for (std::size_t i = 0; i < sp; i++)
				b0.Buffer[i] = i < bSize ? b.Buffer[i] : 0;

			// B1
			BigInt_T b1;
			Resize(b1, sp + 2); // Plus 2 for the possible carry in the later sum
			for (std::size_t i = sp; i < size; i++)
				b1.Buffer[i - sp] = i < bSize ? b.Buffer[i] : 0;

			// K1
			BigInt_T k1;
			std::size_t a1Size = CountSignificantWords(a1);
			Resize(k1, a1Size + CountSignificantWords(b1) + 2 * sp);
			bi_memcpy(k1.Buffer, k1.Size * sizeof(WORD), a1.Buffer, a1Size * sizeof(WORD));
			Karatsuba(k1, b1);

			// K2
			BigInt_T k2;
			AddU(a1, a0);
			AddU(b1, b0);
			a1Size = CountSignificantWords(a1);
			Resize(k2, a1Size + CountSignificantWords(b1) + sp);
			bi_memcpy(k2.Buffer, k2.Size * sizeof(WORD), a1.Buffer, a1Size * sizeof(WORD));
			Karatsuba(k2, b1);

			// K3
			BigInt_T k3;
			std::size_t a0Size = CountSignificantWords(a0);
			Resize(k3, a0Size + CountSignificantWords(b0));
			bi_memcpy(k3.Buffer, k3.Size * sizeof(WORD), a0.Buffer, a0Size * sizeof(WORD));
			Karatsuba(k3, b0);

			SubU(k2, k3);
			SubU(k2, k1);

			ShiftLeft(k1, 16 * sp * sizeof(WORD));
			ShiftLeft(k2, 8 * sp * sizeof(WORD));

			memset(a.Buffer, 0, a.Size * sizeof(WORD));
			AddU(a, k1);
			AddU(a, k2);
			AddU(a, k3);
		};

		// Toom-Cook 3-Way algorithm
		static std::function<void(BigInt_T&, const BigInt_T&)> ToomCook3
		=
		[](BigInt_T& a, const BigInt_T& b) {

			// Divide by 3
			auto DivideBy3
			=
			[](BigInt_T& a) {

				constexpr WORD BASE = (WORD)1 << (sizeof(WORD) * 4);

				HALFWORD* buffer = (HALFWORD*)a.Buffer;
				const std::size_t n = a.Size * 2;
				constexpr WORD d = 2'863'311'531; // Precomputed inverse module between 3 and 2^32
				HALFWORD b = 0;
				for (size_t i = 0; i < n; ++i) {

					WORD ai = buffer[i];
					WORD x, b1;
					if (b <= ai) {

						x = ai - b;
						b1 = 0;
					}

					else {

						x = ai + BASE - b;
						b1 = 1;
					}

					WORD q = (d * x) % BASE;
					buffer[i] = (HALFWORD)q;

					WORD prod = (WORD)q * 3;
					WORD b2 = (prod - x) / BASE;
					b = (HALFWORD)(b1 + (HALFWORD)b2);
				}

				return b;
			};

			const std::size_t aSize = CountSignificantWords(a);
			const std::size_t bSize = CountSignificantWords(b);

			if (aSize < 30000 || bSize < 30000) {

				// If the first factor is equal to zero, do not bother executing the algorithm
				if (aSize == 1)
					if (a.Buffer[0] == 0)
						return;

				// Same thing for the other factor
				if (bSize == 1) {

					if (b.Buffer[0] == 0) {

						memset(a.Buffer, 0, a.Size * sizeof(WORD));

						return;
					}
				}

				Karatsuba(a, b);

				return;
			}

			// Split

			const std::size_t size = std::max(aSize, bSize);
			const std::size_t k = (std::size_t)std::ceil((long double)size / 3.0);

			BigInt_T a0, a1, a2, b0, b1, b2;
			Resize(a0, k);
			Resize(a1, k + 1);
			Resize(a2, size - 2 * k + 1);
			Resize(b0, k);
			Resize(b1, k + 1);
			Resize(b2, size - 2 * k + 1);

			// A0
			for (std::size_t i = 0; i < k; i++)
				a0.Buffer[i] = i < aSize ? a.Buffer[i] : 0;

			// A1
			for (std::size_t i = k; i < 2 * k; i++)
				a1.Buffer[i - k] = i < aSize ? a.Buffer[i] : 0;

			// A2
			for (std::size_t i = 2 * k; i < size; i++)
				a2.Buffer[i - 2 * k] = i < aSize ? a.Buffer[i] : 0;

			// B0
			for (std::size_t i = 0; i < k; i++)
				b0.Buffer[i] = i < bSize ? b.Buffer[i] : 0;

			// B1
			for (std::size_t i = k; i < 2 * k; i++)
				b1.Buffer[i - k] = i < bSize ? b.Buffer[i] : 0;

			// B2
			for (std::size_t i = 2 * k; i < size; i++)
				b2.Buffer[i - 2 * k] = i < bSize ? b.Buffer[i] : 0;

			// A02
			BigInt_T a02;
			Resize(a02, std::max(CountSignificantWords(a0), CountSignificantWords(a2)) + 1);
			Copy(a02, a0);
			AddU(a02, a2);

			// B02
			BigInt_T b02;
			Resize(b02, std::max(CountSignificantWords(b0), CountSignificantWords(b2)) + 1);
			Copy(b02, b0);
			AddU(b02, b2);

			// A012
			BigInt_T a012;
			Resize(a012, std::max(CountSignificantWords(a02), CountSignificantWords(a1)) + 1);
			Copy(a012, a02);
			AddU(a012, a1);

			// B012
			BigInt_T b012;
			Resize(b012, std::max(CountSignificantWords(b02), CountSignificantWords(b1)) + 1);
			Copy(b012, b02);
			AddU(b012, b1);

			// V0
			BigInt_T v0;
			Resize(v0, CountSignificantWords(a0) + CountSignificantWords(b0));
			Copy(v0, a0);
			ToomCook3(v0, b0);

			// V1
			BigInt_T v1;
			Resize(v1, CountSignificantWords(a012) + CountSignificantWords(b012));
			Copy(v1, a012);
			ToomCook3(v1, b012);

			// Vm1
			BigInt_T vm1;
			Sub(a02, a1);
			Sub(b02, b1);
			Resize(vm1, CountSignificantWords(a02) + CountSignificantWords(b02) + 1);
			Copy(vm1, a02);
			ToomCook3(vm1, b02);
			vm1.Sign = a02.Sign ^ b02.Sign;

			// a1 -> 2*a1
			ShiftLeft(a1, 1);

			// a2 -> 4*a2
			ShiftLeft(a2, 2);

			// b1 -> 2*b1
			ShiftLeft(b1, 1);

			// b2 -> 4*b2
			ShiftLeft(b2, 2);

			// A0_2A1_4A2
			BigInt_T a0_2a1_4a2;
			Resize(a0_2a1_4a2, std::max(CountSignificantWords(a0), std::max(CountSignificantWords(a1), CountSignificantWords(a2))) + 1);
			Copy(a0_2a1_4a2, a0);
			AddU(a0_2a1_4a2, a1);
			AddU(a0_2a1_4a2, a2);

			// B0_2B1_4B2
			BigInt_T b0_2b1_4b2;
			Resize(b0_2b1_4b2, std::max(CountSignificantWords(b0), std::max(CountSignificantWords(b1), CountSignificantWords(b2))) + 1);
			Copy(b0_2b1_4b2, b0);
			AddU(b0_2b1_4b2, b1);
			AddU(b0_2b1_4b2, b2);

			// 4*a2 -> a2
			ShiftRight(a2, 2);

			// 4*b2 -> b2
			ShiftRight(b2, 2);

			// V2
			BigInt_T v2;
			Resize(v2, CountSignificantWords(a0_2a1_4a2) + CountSignificantWords(b0_2b1_4b2));
			Copy(v2, a0_2a1_4a2);
			ToomCook3(v2, b0_2b1_4b2);

			// Vinf
			BigInt_T vinf;
			Resize(vinf, CountSignificantWords(a2) + CountSignificantWords(b2) + 1);
			Copy(vinf, a2);
			ToomCook3(vinf, b2);

			// vm1 -> 2*vm1
			ShiftLeft(vm1, 1);

			// vinf -> 2*vinf
			ShiftLeft(vinf, 1);

			// 3*V0
			BigInt_T _3v0;
			Resize(_3v0, std::max(CountSignificantWords(v0) + 1, std::max(CountSignificantWords(vm1), CountSignificantWords(v2))) + 1);
			Copy(_3v0, v0);
			MultiplyByWord(_3v0, 3); // Multiply by 3
			Add(_3v0, vm1);
			Add(_3v0, v2);
			ShiftRight(_3v0, 1); // Division by 2
			DivideBy3(_3v0); // Division by 3

			// T1
			BigInt_T t1;
			Resize(t1, std::max(CountSignificantWords(_3v0), CountSignificantWords(vinf)));
			Copy(t1, _3v0);
			Sub(t1, vinf);

			// 2*vm1 -> vm1
			ShiftRight(vm1, 1);

			// 2*vinf -> vinf
			ShiftRight(vinf, 1);

			// T2
			BigInt_T t2;
			Resize(t2, std::max(CountSignificantWords(v1), CountSignificantWords(vm1)) + 1);
			Copy(t2, v1);
			Add(t2, vm1);
			ShiftRight(t2, 1);

			// C0
			const BigInt_T& c0 = v0;

			// C1
			BigInt_T c1;
			Resize(c1, std::max(CountSignificantWords(v1), CountSignificantWords(t1)) + k);
			Copy(c1, v1);
			Sub(c1, t1);

			// C2
			BigInt_T c2;
			Resize(c2, std::max(CountSignificantWords(t2), std::max(CountSignificantWords(v0), CountSignificantWords(vinf))) + 2 * k);
			Copy(c2, t2);
			Sub(c2, v0);
			Sub(c2, vinf);

			// C3
			BigInt_T c3;
			Resize(c3, std::max(CountSignificantWords(t1), CountSignificantWords(t2)) + 3 * k);
			Copy(c3, t1);
			Sub(c3, t2);

			// C4
			BigInt_T c4;
			Resize(c4, CountSignificantWords(vinf) + 4 * k);
			Copy(c4, vinf);

			memset(a.Buffer, 0, a.Size * sizeof(WORD));
			ShiftLeft(c1, 1 * k * sizeof(WORD) * 8);
			ShiftLeft(c2, 2 * k * sizeof(WORD) * 8);
			ShiftLeft(c3, 3 * k * sizeof(WORD) * 8);
			ShiftLeft(c4, 4 * k * sizeof(WORD) * 8);
			Copy(a, c0);
			Add(a, c1);
			Add(a, c2);
			Add(a, c3);
			Add(a, c4);
		};

		// Reserve space for result
		Resize(first, std::max(std::max(first.Size, second.Size), firstSize + secondSize));

		// Multiply
		ToomCook3(first, second);

		// Establish sign
		first.Sign = first.Sign ^ second.Sign;
	}

	void Div(BigInt_T& a, const BigInt_T& b) {

		// Normalize
	}

	// --- Bitwise functions ---

	void Not(BigInt_T& data) {

		for (std::size_t i = 0; i < data.Size; i++)
			data.Buffer[i] = ~data.Buffer[i];
	}

	void And(BigInt_T& first, const BigInt_T& second) {

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

	void Or(BigInt_T& first, const BigInt_T& second) {

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

	void Xor(BigInt_T& first, const BigInt_T& second) {

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

	void ShiftLeft(BigInt_T& data, std::size_t bit_shift_amount) {

		if (bit_shift_amount > data.Size * sizeof(WORD) * 8) {

			memset(data.Buffer, 0, data.Size * sizeof(WORD));

			return;
		}

		WORD*& buffer = data.Buffer;
		std::size_t offset = bit_shift_amount / (sizeof(WORD) * 8);
		std::size_t rest = bit_shift_amount % (sizeof(WORD) * 8);

		if (offset != 0) {

			bi_memmove(buffer + offset, data.Size * sizeof(WORD), buffer, data.Size * sizeof(WORD) - offset * sizeof(WORD));
			memset(buffer, 0, offset * sizeof(WORD));
		}

		if (rest == 0)
			return;

		WORD* word;
		std::size_t size = data.Size;
		for (word = size - 1 + buffer; size--; word--) {

			WORD bits = 0;
			if (size)
				bits = word[-1] & (BI_MAX_WORD << (sizeof(WORD) * 8 - rest));

			*word <<= rest;
			*word |= (bits >> (sizeof(WORD) * 8 - rest));
		}
	}

	void ShiftRight(BigInt_T& data, std::size_t bit_shift_amount) {

		if (bit_shift_amount >= data.Size * sizeof(WORD) * 8) {

			memset(data.Buffer, 0, data.Size * sizeof(WORD));

			return;
		}

		WORD*& buffer = data.Buffer;
		std::size_t offset = bit_shift_amount / (sizeof(WORD) * 8);
		std::size_t rest = bit_shift_amount % (sizeof(WORD) * 8);

		if (offset != 0) {

			bi_memmove(buffer, data.Size * sizeof(WORD), buffer + offset, data.Size * sizeof(WORD) - offset);
			memset(buffer + data.Size - offset, 0, offset * sizeof(WORD));
		}

		if (rest == 0)
			return;

		// Shift the last 'rest' bits to the right

		WORD* word;
		std::size_t size = data.Size;
		for (word = data.Buffer; size--; word++) {

			WORD bits = 0;
			if (size)
				bits = word[1] & (BI_MAX_WORD >> (sizeof(WORD) * 8 - rest));

			*word >>= rest;
			*word |= (bits << (sizeof(WORD) * 8 - rest));
		}
	}

	// --- String functions ---

	std::string ToString(const BigInt_T& data) {

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
		const std::size_t significantBits = CountSignificantBits(data);

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

	bool FromString(BigInt_T& data, const std::string& str) {

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

		const std::size_t dataSize = (std::size_t)std::ceil(std::ceil((long double)strLength * log2(10.0l)) / (sizeof(WORD) * 8.0l));
		bool capacityAlreadySet = false; // To remove unnecessary padding caused by the instruction up here. But if the capacity was stored by the user, keep the padding at the end of this function

		if (data.Size < dataSize) {

			Clear(data);
			Resize(data, dataSize);
		}

		else {

			capacityAlreadySet = true;
			memset(data.Buffer, 0, data.Size * sizeof(WORD));
		}

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

		if (!capacityAlreadySet)
			ShrinkToFit(data);

		return true;
	}
}
