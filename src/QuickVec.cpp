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

	{
		auto start = clock::now();
		static const uint32_t RESOLUTION = 1024*4;
		static const uint32_t ITERATIONS = 10;
		bool* vals= new bool[RESOLUTION*RESOLUTION];
		for (int iy = 0; iy < RESOLUTION; iy++) {
			for (int ix = 0; ix < RESOLUTION; ix++) {
				float x = static_cast<float>(ix) / RESOLUTION;
				float y = static_cast<float>(iy) / RESOLUTION;
				float z = 0.0f;
				float zi = 0.0f;
				for (int i = 0; i < ITERATIONS; i++) {
					if (((z*z) + (zi*zi)) < 4.0f) {
						break;
					}
					z = z*z + zi*zi + x;
					zi = 2.0f*z*zi + y;
				}
				if (((z*z) + (zi*zi))>4.0f)
					vals[ix + iy*RESOLUTION] = true;
				else
					vals[ix + iy*RESOLUTION] = false;
			}
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Mandelbrot took " << duration.count() << "s\n";
		delete[] vals;
	}

	{
		auto start = clock::now();
		uint32_t RESOLUTION = 1024 * 4;
		uint32_t ITERATIONS = 10;
		bool* vals = new bool[RESOLUTION*RESOLUTION];
		for (int iy = 0; iy < RESOLUTION; iy++) {
			QuickVec::float4_sse4_1 y(static_cast<float>(iy) / RESOLUTION);
			for (int ix = 0; ix +3 < RESOLUTION; ix+=4) {
				QuickVec::float4_sse4_1 x(ix, ix + 1, ix + 2, ix + 3);
				x /= RESOLUTION;
				QuickVec::float4_sse4_1 z, zi; //auto set to zero
				for (int i = 0; i < ITERATIONS; i++) {
					QuickVec::float4_sse4_1::bool_t is_finished = (z*z + zi*zi) < 4.0f;
					if (is_finished.all()) break;
					z = z*z + zi*zi + x;
					zi = z*zi*2.0f + y;
				}
			}
		}
		auto end = clock::now();
		time_diff duration = end - start;
		std::cout << "Mandelbrot took " << duration.count() << "s\n";
		delete[] vals;
	}

	std::cin.ignore();

	return 0;
}

