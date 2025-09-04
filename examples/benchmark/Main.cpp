#include <iostream>
#include <fstream>
#include <chrono>

#include <BigInteger.hpp>

class Timer {

public:

	Timer() {

		start_time = std::chrono::high_resolution_clock::now();
	}

	void reset() {

		start_time = std::chrono::high_resolution_clock::now();
	}

	double elapsedNanoseconds() const {

		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::nano> duration = end_time - start_time;
		return duration.count();
	}

	double elapsedMicroseconds() const {

		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> duration = end_time - start_time;
		return duration.count();
	}

	double elapsedMilliseconds() const {

		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = end_time - start_time;
		return duration.count();
	}

	double elapsedSeconds() const {

		auto end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end_time - start_time;
		return duration.count();
	}

private:

	std::chrono::high_resolution_clock::time_point start_time;
};

static void factorial(std::size_t n) {

	big::Integer r(1);
	for (std::size_t i = 1; i <= n; i++)
		r *= i;
}

static std::ofstream fout;
static std::size_t bytes = 0;

void* _Post_writable_byte_size_(_Size) operator new(std::size_t size) {

	bytes += size;
	void* p = std::malloc(size);
	if (!p) throw std::bad_alloc();

	return p;
}

static void DumpSumInfo(std::size_t bits) {

	big::Integer a(0, bits);
	big::Integer::Rand(a);
	big::Integer b(0, bits);
	big::Integer::Rand(b);
	std::cout << "(" << bits << " + " << bits << ")...";
	Timer timer;
	bytes = 0;
	a + b;
	double time = timer.elapsedMicroseconds();
	fout << bits << ";" << time << ";" << bytes << "\n";
	fout.flush();
	std::cout << " Done! (" << time << (unsigned char)230 << "s)\n";
}

static void DumpMultInfo(std::size_t bits) {

	big::Integer a(0, bits);
	big::Integer::Rand(a);
	big::Integer b(0, bits);
	big::Integer::Rand(b);
	std::cout << "(" << bits << " * " << bits << ")...";
	Timer timer;
	bytes = 0;
	a* b;
	double time = timer.elapsedMicroseconds();
	fout << bits << ";" << time << ";" << bytes << "\n";
	fout.flush();
	std::cout << " Done! (" << time << (unsigned char)230 << "s)\n";
}

static void DumpFactInfo(std::size_t bits) {

	std::cout << "factorial(" << bits << ")...";
	Timer timer;
	bytes = 0;
	factorial(bits);
	double time = timer.elapsedMicroseconds();
	fout << bits << ";" << time << ";" << bytes << "\n";
	fout.flush();
	std::cout << " Done! (" << time << (unsigned char)230 << "s)\n";
}

int main() {

	// Sum
	std::cout << "--- Sum data ---\n";
	fout.open("data-sum.csv");
	if (!fout.is_open())
		return -1;

	for (std::size_t i = 1; i <= 4096; i++)
		DumpSumInfo(i * 64);

	// Multiplication
	std::cout << "--- Multiplication data ---\n";
	fout.close();
	fout.open("data-mult.csv");
	if (!fout.is_open())
		return -1;

	for (std::size_t i = 1; i <= 4096; i++)
		DumpMultInfo(i * 64);

	// Factorial
	std::cout << "--- Factorial data ---\n";
	fout.close();
	fout.open("data-fact.csv");
	if (!fout.is_open())
		return -1;

	for (std::size_t i = 1; i <= 20000; i++)
		DumpFactInfo(i);

	fout.close();

	// Numbers to be copied to freely test edge cases
	// 18446744073709551615
	// 9223372036854775807
	// 4294967295
	// 179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215
	// 32317006071311007300714876688669951960444102669715484032130345427524655138867890893197201411522913463688717960921898019494119559150490921095088152386448283120630877367300996091750197750389652106796057638384067568276792218642619756161838094338476170470581645852036305042887575891541065808607552399123930385521914333389668342420684974786564569494856176035326322058077805659331026192708460314150258592864177116725943603718461857357598351152301645904403697613233287231227125684710820209725157101726931323469678542580656697935045997268352998638215525166389437335543602135433229604645318478604952148193555853611059596230655

	big::Integer n1(0, 1024);
	big::Integer::Rand(n1);
	big::Integer n2(0, 1024);
	big::Integer::Rand(n2);

	int i = 0;
	big::Integer n(0, 2048);
	Timer timer;
	while (i < 1'000'000) { //1'000'000

		n = n1 * n2;
		i++;
	}

	double time = timer.elapsedNanoseconds() / 1'000'000.0;
	std::cout << time << "ns\n";
	std::cout << std::fixed << (std::size_t)std::ceil(1.0 / (time / 1'000'000'000)) << " operation per second\n";
	std::cin.get();

	return 0;
}
