#include <iostream>

#include <BigInteger.hpp>

int main() {

	PRINT("Test %d", 1);
	big::Integer n1(0, 1024);
	std::cin >> n1;
	std::cout << n1 << "\n";

	return 0;
}
