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

namespace Utils {

	/// <summary>
	/// Debug function to print on the standard output the binary value of an array
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	void PrintAsBinary(void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Resizes the given array of integers. Can be used to create or resize an array while keeping its original content (as long as the new size is greater or equal to the old one).
	/// To make it work with 2's complement, this function can automatically extend the sign
	/// </summary>
	/// <param name="data">The array to resize</param>
	/// <param name="new_size">The new size</param>
	/// <param name="ext_sign">If the number should be sign extended (true by default)</param>
	void Resize(bi_int& data, std::size_t new_size, bool ext_sign = true);

	/// <summary>
	/// Copies the src big integer into the dest one
	/// </summary>
	/// <param name="dest">The destination</param>
	/// <param name="src">The source</param>
	/// <param name="offset_dest">The destination offset (zero by default)</param>
	void Copy(bi_int& dest, const bi_int& src, const std::size_t offset_dest = 0, bool ext_sign = true);

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
	/// Converts the first 8 bytes of the given array into a qword (64 bit word)
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The desired array size in bytes</param>
	/// <returns>The qword</returns>
	std::uint64_t BytesToQWORD(const void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Converts the first 8 bytes of the given array into a signed qword (64 bit word)
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The desired array size in bytes</param>
	/// <returns>The signed qword</returns>
	std::int64_t BytesToSignedQWORD(const void* data, std::size_t size_in_bytes);

	bi_int SignedQWORDToBytes(std::int64_t n);

	/// <summary>
	/// Gets the minimum number of bits needed to represent the desired number
	/// </summary>
	/// <param name="n">The desired number</param>
	/// <returns>The minimum number of bits needed to represent the desired number</returns>
	std::size_t GetBitSize(std::uint64_t n);

	/// <summary>
	/// Gets the minimum number of bytes needed to represent the desired number
	/// </summary>
	/// <param name="n">The desired number</param>
	/// <returns>The minimum number of bytes needed to represent the desired number</returns>
	std::size_t GetByteSize(std::uint64_t n);

	/// <summary>
	/// Checks whether the specified number is negative or not
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>True if the number is negative, false if not</returns>
	bool IsNegative(const bi_int& data);

	/// <summary>
	/// Counts the number of bits that are not considered padding
	/// </summary>
	/// <param name="n">The raw big integer</param>
	/// <param name="size">The raw big integer size</param>
	/// <returns>The number of bits that have an impact on the number representation</returns>
	std::size_t CountSignificantBits(const bi_type* const n, std::size_t size);

	// --- Mathematical operations ---

	/// <summary>
	/// Compares the two given big integers
	/// </summary>
	/// <param name="first">The first big integer</param>
	/// <param name="second">The second big integer</param>
	/// <returns>
	/// A number greater than zero if the first number is bigger than the second one.
	/// A number smaller than zero if the first number is smaller than the second one.
	/// Zero if the two numbers are equal
	/// </returns>
	int Compare(const bi_int& first, const bi_int& second);

	/// <summary>
	/// Negates the big integer. If the big integer was positive, it will became negative. If it was negative, it will become positive
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Negate(bi_int& data);

	/// <summary>
	/// Turns every number into a positive number
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
	/// Sums the first big integer with the second one. The result is stored in the first one. The first array could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="first">The first addend, where the result will be stored</param>
	/// <param name="second">The second addend</param>
	void Add(bi_int& first, const bi_int& second);

	/// <summary>
	/// Multiplies the first big integer with the second one. The result is stored in the first one. The first array could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="first">The first factor, where the result will be stored</param>
	/// <param name="second">The second factor</param>
	void Mult(bi_int& first, const bi_int& second);

	// --- Bitwise operations ---

	/// <summary>
	/// Inverts all the bits in the given array
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	void Not(void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Shifts all the bits to the left by a specified amount
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	/// <param name="shift_amount">The shift amount in bits</param>
	void ShiftLeft(void* data, std::size_t size_in_bytes, std::size_t shift_amount);

	/// <summary>
	/// Shifts all the bits to the right by a specified amount
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	/// <param name="shift_amount">The shift amount in bits</param>
	void ShiftRight(void* data, std::size_t size_in_bytes, std::size_t shift_amount);

	/// <summary>
	/// Shifts all the bits to the left by one
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	/// <exception cref="THIS MUST NOT BE USED WITH REGULAR BIG INTEGERS"></exception>
	void ShiftLeft1BE(void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Shifts all the bits to the right by one
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	/// <exception cref="THIS MUST NOT BE USED WITH REGULAR BIG INTEGERS"></exception>
	void ShiftRight1BE(void* data, std::size_t size_in_bytes);
}
