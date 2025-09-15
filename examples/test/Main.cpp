#include <iostream>

#include <BigInteger.hpp>

int main() {

	std::cout << "Type the number to select an operation:\n";
	std::cout << " 1. (+)\n";
	std::cout << " 2. (-)\n";
	std::cout << " 3. (x)\n";
	std::cout << " 4. (/)\n";
	std::cout << " 5. (compare)\n";

	std::cout << ">";
	int in;
	std::cin >> in;
	std::cout << "\n";

	big::Integer n1(0);
	big::Integer n2(0);
	if (in >= 1 && in <= 5) {

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
	}

	big::Integer n;
	switch (in) {

		case 1: n = n1 + n2; break;
		case 2: n = n1 - n2; break;
		case 3: n = n1 * n2; break;
		case 4: n = n1 / n2; break;
		case 5:

			if (n1 == n2)
				std::cout << n1 << " == " << n2 << "\n";
			if (n1 != n2)
				std::cout << n1 << " != " << n2 << "\n";
			if (n1 > n2)
				std::cout << n1 << " > " << n2 << "\n";
			if (n1 < n2)
				std::cout << n1 << " < " << n2 << "\n";
			if (n1 >= n2)
				std::cout << n1 << " >= " << n2 << "\n";
			if (n1 <= n2)
				std::cout << n1 << " <= " << n2 << "\n";

			return 0;

		default:
			std::cout << "Invalid operation\n";
			return 1;
	}

	std::cout << "Result: " << n << "\n";
	std::cout << "Size in bytes: " << n.Size() * sizeof(WORD) << "\n";

	return 0;
}
