#include <iostream>

#include <BigInteger.hpp>

int main() {

	// 18446744073709551615
	// 9223372036854775807

	PRINT("Test %d", 1);
	big::Integer n1(0);
	big::Integer n2(0);
	std::cin >> n1 >> n2;
	if (std::cin.fail())
		std::cerr << "ERROR\n";
	else
		std::cout << n1 + n2 << "\n";
	//std::cout << n1 << " (Size: " << n1.Size() << " - Capacity: " << n1.Capacity() << ")\n";

	return 0;
}
