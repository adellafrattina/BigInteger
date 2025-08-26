#include <iostream>

#include <BigInteger.hpp>

int main() {

	// 18446744073709551615
	// 9223372036854775807

	PRINT("Test %d", 1);
	big::Integer n1(0, 1024);
	++n1;
	std::cout << n1 << " (Size: " << n1.Size() << " - Capacity: " << n1.Capacity() << ")\n";

	return 0;
}
