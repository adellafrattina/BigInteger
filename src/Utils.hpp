#include <cstdlib>
#include <cstdint>

// For debug purposes
#undef PRINT
#if !defined(BI_PRINT_ACTIVE)
	#define PRINT(fmt, ...)
#else
	// To print debug information on standard output
	#define PRINT(fmt, ...) printf(fmt, __VA_ARGS__); putchar('\n');
#endif

namespace Utils {

	/// <summary>
	/// Debug function to print on the standard output the binary value of an array
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The array size in bytes</param>
	void PrintAsBinary(void* data, std::size_t size_in_bytes);

	/// <summary>
	/// Resizes the given array of integers. Can be used to create or resize an array while keeping its original content (as long as the new size is greater or equal to the old one)
	/// </summary>
	/// <param name="data">The array to resize</param>
	/// <param name="old_size">The old size</param>
	/// <param name="new_size">The new size</param>
	void Resize(std::uint32_t*& data, std::size_t old_size, std::size_t new_size);

	// --- Mathematical operations ---

	/// <summary>
	/// Increments the big integer by one. It could resize the array if overflow occurs
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <param name="size">The big integer size</param>
	/// <returns>The new big integer size if the increment causes overflow</returns>
	std::size_t Increment(std::uint32_t*& data, std::size_t size);

	/// <summary>
	/// Sums the first big integer with the second one. The result is stored in the first one. The first array could be resized if
	/// the second one is bigger or if overflow occurs
	/// </summary>
	/// <param name="data_dest">The first addend, where the result will be stored</param>
	/// <param name="size_dest">The first addend size</param>
	/// <param name="data_to_sum">The second addend</param>
	/// <param name="size_to_sum">The second added size</param>
	/// <returns>The new big integer size if the increment causes overflow</returns>
	std::size_t Add(std::uint32_t*& data_dest, std::size_t size_dest, std::uint32_t* data_to_sum, std::size_t size_to_sum);

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
