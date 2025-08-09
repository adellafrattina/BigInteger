#include <iostream>

#include <BigInteger.hpp>

int main() {

	PRINT("Test %d", 1);
	big::Integer n1(9223372036854775807, 8); //18446744073709551615 //9223372036854775807
	big::Integer n2(-9223372036854775807, 12); //18446744073709551615 //9223372036854775807
	std::cin >> n1;

	//n1.Resize(64);
	//n.ShrinkToFit();

	std::cout << n1 + n2 << "\n";

	return 0;
}
