#include <iostream>

#include <BigInteger.hpp>

int main() {

	std::cout << "Type the number to select an operation:\n";
	std::cout << " 1. (+)\n";
	std::cout << " 2. (-)\n";
	std::cout << " 3. (x)\n";
	std::cout << " 4. (/)\n";
	std::cout << " 5. (%)\n";
	std::cout << " 6. (compare)\n";
	std::cout << " 7. (rand)\n";

	std::cout << ">";
	int in;
	std::cin >> in;
	std::cout << "\n";

	big::Integer n1(0);
	big::Integer n2(0);
	if (in >= 1 && in <= 6) {

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
		case 5: n = n1 % n2; break;
		case 6:

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

		case 7: {

			big::Integer::Seed((unsigned)time(NULL));

			big::Integer ub;
			big::Integer lb;

			bool repeat;
			do {

				repeat = false;

				std::cout << "Insert a upper bound: ";
				std::cin >> ub;
				std::cout << "Insert a lower bound: ";
				std::cin >> lb;

				if (ub < 0 || lb == ub || ub < lb) {

					std::cout << "Invalid inputs!\n";
					repeat = true;
				}

			} while (repeat);

			n.Clear();
			n.Resize(ub.Size());
			big::Integer::Rand(n);
			n %= ub;
			n += lb;

			break;
		}

		default:
			std::cout << "Invalid operation\n";
			return 1;
	}

	std::cout << "Result: " << n << "\n";
	std::cout << "Size in bytes: " << n.Size() * sizeof(WORD) << "\n";

	return 0;
}
