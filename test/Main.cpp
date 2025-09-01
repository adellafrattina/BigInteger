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

void* operator new(std::size_t size) {

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
	std::cout << " Done!\n";
}

static void DumpMultInfo(std::size_t bits) {

	big::Integer a(0, bits);
	big::Integer::Rand(a);
	big::Integer b(0, bits);
	big::Integer::Rand(b);
	std::cout << "(" << bits << " * " << bits << ")...";
	Timer timer;
	bytes = 0;
	a * b;
	double time = timer.elapsedMicroseconds();
	fout << bits << ";" << time << ";" << bytes << "\n";
	fout.flush();
	std::cout << " Done!\n";
}

static void DumpFactInfo(std::size_t bits) {

	std::cout << "factorial(" << bits << ")...";
	Timer timer;
	bytes = 0;
	factorial(bits);
	double time = timer.elapsedMicroseconds();
	fout << bits << ";" << time << ";" << bytes << "\n";
	fout.flush();
	std::cout << " Done!\n";
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
	fout.close();
	std::cout << "--- Multiplication data ---\n";
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

	for (std::size_t i = 1; i <= 1000; i++)
		DumpFactInfo(i);

	fout.close();
}
