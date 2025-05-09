#include <iostream>

#include <BigInteger.hpp>

int main() {

	bi::Integer n1(0); // 18'446'744'073'709'551'615 18'000'000'000'009'551'615 18446744073709551615 4294967295 9223372036854775807
	bi::Integer n2(0);
	std::cout << "Insert first number: ";
	std::cin >> n1;

	if (std::cin.fail()) {

		std::cout << "Not a number\n";

		return 1;
	}

	std::cout << "Insert second number: ";
	std::cin >> n2;

	if (std::cin.fail()) {

		std::cout << "Not a number\n";

		return 1;
	}

	bi::Integer n = n1 * n2;
	std::cout << "Result: " << n << "\n";
	std::cout << "Size in bytes: " << n.SizeInBytes() << "\n";
}
