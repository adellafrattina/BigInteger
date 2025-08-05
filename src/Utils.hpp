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
	/// Performs a runtime check to determine the system endianness
	/// </summary>
	/// <returns>True if the system is little-endian, false if it is big-endian</returns>
	inline bool IsLittleEndian();

	/// <summary>
	/// Checks if the big integer buffer is allocated on the stack
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>True if it is allocated on the stack, false if it is allocated on the heap</returns>
	bool IsOnStack(const bi_int& data);

	/// <summary>
	/// Resizes the given big integer. Can be used to create or resize a big integer's buffer while keeping its original content (as long as the new size is greater or equal to the old one).
	/// To make it work with 2's complement, this function can automatically extend the sign
	/// </summary>
	/// <param name="data">The big integer to create/resize</param>
	/// <param name="new_size">The new size</param>
	/// <param name="ext_sign">If the number should be sign extended (true by default)</param>
	void Resize(bi_int& data, std::size_t new_size, bool ext_sign = true);

	/// <summary>
	/// Copies the src big integer into the dest one
	/// </summary>
	/// <param name="dest">The destination (where to store the src)</param>
	/// <param name="src">The source (from where to pick the data)</param>
	/// <param name="offset_dest">The destination offset (zero by default)</param>
	/// <param name="ext_sign">If the destination buffer is bigger than the source one, the destination will be sign extended at the end (true by default)</param>
	void Copy(bi_int& dest, const bi_int& src, const std::size_t offset_dest = 0, bool ext_sign = true);

	/// <summary>
	/// Moves the src big integer into the dest one. If the src is allocated on the stack, it will be trivially copied
	/// </summary>
	/// <param name="dest">The destination (where to store the src)</param>
	/// <param name="src">The source (from where to pick the data)</param>
	/// <param name="ext_sign">If the destination buffer is bigger than the source one, the destination will be sign extended at the end (true by default)</param>
	void Move(bi_int& dest, bi_int& src, bool ext_sign = true);

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
	/// Gets the minimum number of bits needed to represent the desired number
	/// </summary>
	/// <param name="n">The desired number</param>
	/// <returns>The minimum number of bits needed to represent the desired number</returns>
	std::size_t GetBitSize(WORD n);

	/// <summary>
	/// Gets the minimum number of bytes needed to represent the desired number
	/// </summary>
	/// <param name="n">The desired number</param>
	/// <returns>The minimum number of bytes needed to represent the desired number</returns>
	std::size_t GetByteSize(WORD n);

	/// <summary>
	/// Converts the first bytes (usually 8 bytes) of a big integer to a word
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>The word resulted from the first bytes of the specified big integer</returns>
	inline WORD BigIntegerToWORD(const bi_int& data);

	/// <summary>
	/// Converts a word (usually 8 bytes integer) to a big integer.
	/// Creating a big integer with this function has the advantage of always instantiating the buffer on the stack
	/// </summary>
	/// <param name="word">The desired word</param>
	/// <returns>The big integer resulted from the specified word</returns>
	inline bi_int BigIntegerFromWORD(WORD word);

	/// <summary>
	/// Converts the given array bytes into a WORD (8 bytes on 64-bit architecture, 4 bytes on 32-bit architecture).
	/// The byte ordering must be little-endian, so the less significant bits (LSBs) will derive from the first element in the array, while the most significant bits (MSBs) will derive from the last element in the array
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The desired array size in bytes</param>
	/// <returns>The word</returns>
	WORD BytesToWORD(const std::uint8_t* data, std::size_t size_in_bytes);

	/// <summary>
	/// Converts the given word (8 bytes on 64-bit architecture, 4 bytes on 32-bit architecture) into an array of bytes.
	/// The byte ordering will be little-endian, so the less significant bit (LSB) will be inside the first element in the array, while the most significant bit (MSB) will be inside the last element in the array
	/// </summary>
	/// <param name="data">The desired array</param>
	/// <param name="size_in_bytes">The desired array size in bytes</param>
	/// <param name="word">The desired word</param>
	void BytesFromWORD(std::uint8_t* data, std::size_t size_in_bytes, const WORD word);

	/// <summary>
	/// Checks whether the specified number is negative or not
	/// </summary>
	/// <param name="data">The desired big integer</param>
	/// <returns>True if the number is negative, false if not</returns>
	inline bool IsNegative(const bi_int& data);

	/// <summary>
	/// Counts the number of bits that are not considered padding
	/// </summary>
	/// <param name="data">The big integer buffer</param>
	/// <param name="size">The big integer buffer size</param>
	/// <returns>The number of bits that have an impact on the number representation</returns>
	std::size_t CountSignificantBits(const bi_type* const data, std::size_t size);

	// --- Mathematical functions ---

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
	/// <param name="first">The first addend, where the result will be stored</param>
	/// <param name="second">The second addend</param>
	void Add(bi_int& first, const bi_int& second);

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
	/// <param name="ext_sign">If the shift should extend the sign</param>
	void ShiftRight(bi_int& data, std::size_t bit_shift_amount, bool ext_sign = false);

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
