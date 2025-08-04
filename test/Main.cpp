#include <iostream>

#include <BigInteger.hpp>

int main() {

	PRINT("Test %d", 1);
	big::Integer n(-9223372036854775807, 9); //18446744073709551615 //9223372036854775807

	std::cout << n;

	return 0;
}
