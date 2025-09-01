#include <cstdlib>
#include <cstdint>
#include <memory>

#include <BigInteger.hpp>

#undef bi_memcpy
#if defined(_WIN32)
	#define bi_memcpy(dest, dest_size, src, src_size) memcpy_s(dest, dest_size, src, src_size)
#else
	#define bi_memcpy(dest, dest_size, src, src_size) std::memcpy(dest, src, src_size)
#endif

#undef bi_memmove
#if defined(_WIN32)
	#define bi_memmove(dest, dest_size, src, src_size) memmove_s(dest, dest_size, src, src_size)
#else
	#define bi_memmove(dest, dest_size, src, src_size) std::memmove(dest, src, src_size)
#endif

constexpr std::uint8_t HIGH_BITS = 0xF0;
constexpr std::uint8_t LOW_BITS = 0x0F;

namespace Utils {

	// --- Debug functions ---

	/// <summary>
	/// Debug function to print on the standard output the binary value of an array
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	void PrintAsBinary(void* data, std::size_t size_in_bytes);

	// --- Basic functions ---

	/// <summary>
	/// Checks if the big integer buffer is allocated on the stack
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>True if it is allocated on the stack, false if it is allocated on the heap</returns>
	static inline bool IsOnStack(const bi_int& data) {

		return data.Size == 1;
	}

	/// <summary>
	/// Resizes the given big integer. Can be used to create or resize a big integer's buffer while keeping its original content (as long as the new size is greater or equal to the old one).
	/// </summary>
	/// <param name="data">The big integer to create/resize</param>
	/// <param name="new_size">The new size</param>
	void Resize(bi_int& data, std::size_t new_size);

	/// <summary>
	/// Copies the src big integer into the dest one
	/// </summary>
	/// <param name="dest">The destination (where to store the src)</param>
	/// <param name="src">The source (from where to pick the data)</param>
	/// <param name="offset_dest">The destination offset (zero by default)</param>
	/// <param name="offset_src">The source offset (zero by default)</param>
	void Copy(bi_int& dest, const bi_int& src, const std::size_t offset_dest = 0, const std::size_t offset_src = 0);

	/// <summary>
	/// Moves the src big integer into the dest one. If the src is allocated on the stack, it will be trivially copied
	/// </summary>
	/// <param name="dest">The destination (where to store the src)</param>
	/// <param name="src">The source (from where to pick the data)</param>
	void Move(bi_int& dest, bi_int& src);

	/// <summary>
	/// Clears the big integer
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Clear(bi_int& data);

	/// <summary>
	/// Removes unnecessary padding bytes
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void ShrinkToFit(bi_int& data);

	/// <summary>
	/// Counts the number of bits that are not considered padding
	/// </summary>
	/// <param name="data">The big integer data</param>
	/// <returns>The number of bits that have an impact on the number representation</returns>
	std::size_t CountSignificantBits(const bi_int& data);

	/// <summary>
	/// Counts the number of words that are not considered padding
	/// </summary>
	/// <param name="data">The big integer data</param>
	/// <returns>The number of words that have an impact on the number representation</returns>
	std::size_t CountSignificantWords(const bi_int& data);

	// --- Mathematical functions ---

	/// <summary>
	/// Compares the two given big integers
	/// </summary>
	/// <param name="a">The first big integer</param>
	/// <param name="b">The second big integer</param>
	/// <returns>
	/// A number greater than zero if the first number is bigger than the second one.
	/// A number smaller than zero if the first number is smaller than the second one.
	/// Zero if the two numbers are equal
	/// </returns>
	int Compare(const bi_int& a, const bi_int& b);

	/// <summary>
	/// Compares the two given big integers' magnitude
	/// </summary>
	/// <param name="a">The first big integer</param>
	/// <param name="b">The second big integer</param>
	/// <returns>
	/// A number greater than zero if the first number is bigger than the second one.
	/// A number smaller than zero if the first number is smaller than the second one.
	/// Zero if the two numbers are equal
	/// </returns>
	int CompareU(const bi_int& a, const bi_int& b);

	/// <summary>
	/// Negates the big integer. If the big integer was positive, it will became negative. If it was negative, it will become positive
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Negate(bi_int& data);

	/// <summary>
	/// Turns the provided big integer number into a positive big integer
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Abs(bi_int& data);

	/// <summary>
	/// Increments the big integer by one. It could resize the array if overflow occurs
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Increment(bi_int& data);

	/// <summary>
	/// Decrements the big integer by one. It could resize the array if overflow occurs
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Decrement(bi_int& data);

	/// <summary>
	/// Sums the first big integer with the second one. The result is stored in the first one. The first big integer buffer could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="a">The first addend, where the result will be stored</param>
	/// <param name="b">The second addend</param>
	void Add(bi_int& a, const bi_int& b);

	/// <summary>
	/// Sums the first big integer's magnitude with the second one (no sign handle). The result is stored in the first one. The first big integer buffer could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="a">The first addend, where the result will be stored</param>
	/// <param name="b">The second addend</param>
	void AddU(bi_int& a, const bi_int& b);

	/// <summary>
	/// Subtracts the first big integer with the second one. The result is stored in the first one. The first big integer buffer could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="a">The minuend and where the result will be stored</param>
	/// <param name="b">The subtrahend</param>
	void Sub(bi_int& a, const bi_int& b);

	/// <summary>
	/// Subtracts the first big integer's magnitude with the second one (no sign handle). The first one must be greater or equal to the second one.
	/// The result is stored in the first one. The first big integer buffer could be resized if
	/// overflow occurs
	/// </summary>
	/// <param name="a">The minuend and where the result will be stored</param>
	/// <param name="b">The subtrahend</param>
	void SubU(bi_int& a, const bi_int& b);

	/// <summary>
	/// Multiplies the first big integer by the second one. The result is stored in the first one. The first big integer buffer could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="a">The first factor, where the result will be stored</param>
	/// <param name="b">The second factor</param>
	void Mult(bi_int& a, const bi_int& b);

	/// <summary>
	/// Divides the first big integer by the second one. The result is stored in the first one. The first big integer buffer could be resized if
	/// overflow occurs
	/// </summary>
	/// <param name="a">The dividend, where the result will be stored</param>
	/// <param name="b">The divisor</param>
	void Div(bi_int& a, const bi_int& b);

	// --- Bitwise functions ---

	/// <summary>
	/// Inverts all the bits in the given big integer
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Not(bi_int& data);

	/// <summary>
	/// Executes the bitwise "and" between the two big integers. The resulting big integer will be stored in the first parameter and
	/// its size will be based on the first big integer's size
	/// </summary>
	/// <param name="first">The first big integer (and where the result will be stored)</param>
	/// <param name="second">The second big integer</param>
	void And(bi_int& first, const bi_int& second);

	/// <summary>
	/// Executes the bitwise "or" between the two big integers. The resulting big integer will be stored in the first parameter and
	/// its size will be based on the first big integer's size
	/// </summary>
	/// <param name="first">The first big integer (and where the result will be stored)</param>
	/// <param name="second">The second big integer</param>
	void Or(bi_int& first, const bi_int& second);

	/// <summary>
	/// Executes the bitwise "xor" between the two big integers. The resulting big integer will be stored in the first parameter and
	/// its size will be based on the first big integer's size
	/// </summary>
	/// <param name="first">The first big integer (and where the result will be stored)</param>
	/// <param name="second">The second big integer</param>
	void Xor(bi_int& first, const bi_int& second);

	/// <summary>
	/// Shifts all the bits to the left by a specified amount
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <param name="bit_shift_amount">The shift amount in bits</param>
	void ShiftLeft(bi_int& data, std::size_t bit_shift_amount);

	/// <summary>
	/// Shifts all the bits to the right by a specified amount
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <param name="bit_shift_amount">The shift amount in bits</param>
	void ShiftRight(bi_int& data, std::size_t bit_shift_amount);

	// --- String functions ---

	/// <summary>
	/// Converts a big integer to a printable string
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>A readable version of the number expressed in base 10 and saved as a string</returns>
	std::string ToString(const bi_int& data);

	/// <summary>
	/// Converts a string to a big integer
	/// </summary>
	/// <param name="data">The big integer variable to store the converted number</param>
	/// <param name="str">The string to convert</param>
	/// <returns>True if the convertion has succeeded, false if not</returns>
	bool FromString(bi_int& data, const std::string& str);
}
