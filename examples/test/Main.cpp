#include <iostream>

#include <BigInteger.hpp>

static bool HasInputFailed() {

	if (std::cin.fail()) {

		std::cout << "Not a number\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		return true;
	}

	return false;
}

int main() {

	bool run = true;
	while (run) {

		std::cout << "Type the number to select an operation:\n";
		std::cout << " 0. (terminate)\n";
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
		if (HasInputFailed()) {

			std::cout << "\n";
			continue;
		}

		big::Integer n1(0);
		big::Integer n2(0);
		if (in >= 1 && in <= 6) {

			bool repeat = false;
			do {

				repeat = false;
				std::cout << "Insert first number: ";
				std::cin >> n1;
				repeat = HasInputFailed();

			} while (repeat);

			do {

				repeat = false;
				std::cout << "Insert second number: ";
				std::cin >> n2;
				repeat = HasInputFailed();

			} while (repeat);
		}

		big::Integer n;
		bool showResult = true;
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

				showResult = false;

				break;

			case 7: {

				big::Integer::Seed((unsigned)time(NULL));

				big::Integer ub;
				big::Integer lb;

				bool repeat;
				do {

					repeat = false;

					do {

						repeat = false;
						std::cout << "Lower bound: ";
						std::cin >> lb;
						repeat = HasInputFailed();

					} while (repeat);

					do {

						repeat = false;
						std::cout << "Upper bound: ";
						std::cin >> ub;
						repeat = HasInputFailed();

					} while (repeat);

					if (ub < 0 || lb == ub || ub < lb) {

						std::cout << "Invalid inputs\n";
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

			case 0:
				showResult = false;
				run = false;
				break;

			default:
				std::cout << "Invalid operation\n";
				showResult = false;
				break;
		}

		if (showResult) {

			std::cout << "Result: " << n << "\n";
			std::cout << "Size in bytes: " << n.Size() * sizeof(WORD) << "\n";
		}

		std::cout << "\n";
	}

	std::cout << "Press enter to terminate...";
	std::cin.ignore();
	std::cin.get();

	return 0;
}
