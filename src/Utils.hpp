#include <cstdlib>
#include <cstdint>

#include "BigInteger.hpp"

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
	/// Converts the first 8 bytes of the given array into a qword (64 bit word)
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The desired array size in bytes</param>
	/// <returns>The qword</returns>
	std::uint64_t BytesToQWORD(const void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Checks whether the specified number is negative or not
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>True if the number is negative, false if not</returns>
	bool IsNegative(const bi_int& data);

	// --- Mathematical operations ---

	/// <summary>
	/// Negates the big integer. If the big integer was positive, it will became negative. If it was negative, it will become positive
	/// </summary>
	/// <param name="data">The desired big integer</param>
	void Negate(bi_int& data);

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
	void ShiftLeft1(void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Shifts all the bits to the right by one
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	void ShiftRight1(void* data, std::size_t size_in_bytes);
}
