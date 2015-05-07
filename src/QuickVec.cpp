#include <tchar.h>
#include <iostream>
#include <chrono>

#include "QuickVec.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	using time_point = std::chrono::time_point<std::chrono::system_clock>;
	using time_diff = std::chrono::duration<double>;
	using clock = std::chrono::system_clock;
	std::cout << std::endl;

	//adds
	{
		using fp = QuickVec::float_base<4>;
		fp f(1, 2, 3, 4);
		fp v = 0;
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v += f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Addition float_base<4> x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	{
		using fp = QuickVec::float4_sse;
		fp f(1, 2, 3, 4);
		fp v = 0;
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v += f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Addition float4_sse x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	// multiply
	{
		using fp = QuickVec::float_base<4>;
		fp f = 1.00001f;
		fp v(1, 2, 3, 4);
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v *= f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Multiply float_base<4> x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	{
		using fp = QuickVec::float4_sse;
		fp f = 1.00001f;
		fp v(1, 2, 3, 4);
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v *= f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Multiply float4_sse x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	// modulo
	{
		using fp = QuickVec::float_base<4>;
		fp f = 1.0002345f;
		fp v(1, 2, 3, 4);
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v %= f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Mod float_base<4> x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	{
		using fp = QuickVec::float4_sse2;
		fp f = 1.0002345f;
		fp v(1, 2, 3, 4);
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v %= f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Mod float4_sse4_1 x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	{
		using fp = QuickVec::float4_sse4_1;
		fp f = 1.0002345f;
		fp v(1, 2, 3, 4);
		auto start = clock::now();
		for (int i = 0; i < 1000000; i++) {
			v %= f;
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Mod float4_sse4_1 x 10^6 took " << duration.count() << "s\n";
		std::cout << "Result is " << v << "\n";
	}

	std::cin.ignore();

	return 0;
}

