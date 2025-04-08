#include <iostream>

#include <BigInteger.hpp>

int main() {

	bi::Integer n(18446744073709551615); // 18'446'744'073'709'551'615 18'000'000'000'009'551'615 18446744073709551615 4294967295
	bi::Integer n2(18446744073709551615);
	bi::Integer n3(18446744073709551615);
	bi::Integer n4(0);

	std::cout << n + n2 << "\n";
}
