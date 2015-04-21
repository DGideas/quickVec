#include <tchar.h>
#include <iostream>
#include <chrono>

#include "QuickVec.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	using time_point = std::chrono::time_point<std::chrono::system_clock>;
	using time_diff = std::chrono::duration<double>;
	using clock = std::chrono::system_clock;
	__m256 a = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	auto b = a;
	auto c = _mm256_add_ps(a, b);
	std::cout << "Results are: ";
	for (auto val : c.m256_f32) {
		std::cout << val << ", ";
	}
	std::cout << std::endl;

	using fp = QuickVec::float_base<8, float>;
	fp f(1, 2, 3, 4, 5, 6, 7, 8);
	fp v = f;
	v += f;
	v = f + v;
	auto start = clock::now();
	for (int i = 0; i < 1000000; i++) {
		v += f;
	}
	auto end = clock::now();
	time_diff duration = end - start;
	std::cout << "V = " << v << std::endl;
	std::cout << "That took " << duration.count() << "s\n";
	std::cout << "Hello. Type some stuff." << std::endl;
	std::cin.ignore();

	return 0;
}

