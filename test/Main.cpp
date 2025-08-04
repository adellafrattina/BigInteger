#include <iostream>

#include <BigInteger.hpp>

int main() {

	PRINT("Test %d", 1);
	big::Integer n(-9223372036854775807, 16); //18446744073709551615 //9223372036854775807

	n.Resize(64);
	n.ShrinkToFit();

	std::cout << n;

	return 0;
}
