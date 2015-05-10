#include <tchar.h>
#include <iostream>
#include <chrono>
#include <memory>

#include "QuickVec.hpp"

using time_point = std::chrono::time_point<std::chrono::system_clock>;
using time_diff = std::chrono::duration<double>;
using my_clock = std::chrono::system_clock;

template<size_t RESOLUTION, size_t ITERATIONS>
void doMandelbrotFloat(float* results) {
	auto start = my_clock::now();
	for (int I = 0; I < 100; I++) {
		float oneOverRes = 1.0f / RESOLUTION;
		for (int iy = 0; iy < RESOLUTION; iy++) {
			float y = static_cast<float>(iy) *oneOverRes;
			for (int ix = 0; ix < RESOLUTION; ix++) {
				float x = static_cast<float>(ix) * oneOverRes;
				float z = 0.0f;
				float zi = 0.0f;
				bool finished = false;
				int i;
				for (i = 0; i < ITERATIONS; i++) {
					z = (z*z) + (zi*zi) + x;
					zi = 2.0f*z*zi + y;
					if (((z*z) + (zi*zi)) < 4.0f) {
						finished = true;
						break;
					}
				}
				//Record values
				results[ix + (iy * RESOLUTION)] = i;
			}
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}

template<typename float_vec, size_t RESOLUTION, size_t ITERATIONS>
void doMandelbrotVector(float* results) {
	auto start = my_clock::now();
	for (int I = 0; I < 100; I++) {
		float_vec increment(0, 1, 2, 3, 4, 5, 6, 7);
		float_vec one = 1.0f;
		float_vec four = 4.0f;
		float_vec oneOverRes = 1.0f / RESOLUTION;
		for (int iy = 0; iy < RESOLUTION; iy++) {
			float_vec y = float_vec(static_cast<float>(iy)) * oneOverRes;
			for (int ix = 0; ix < RESOLUTION; ix += float_vec::size) {
				float_vec x = (increment + ix) * oneOverRes;
				float_vec z, zi, vals;
				for (int i = 0; i < ITERATIONS; i++) {
					z = (z*z) + (zi*zi) + x;
					zi = 2.0f*z*zi + y;
					float_vec::bool_t finished = (((z*z) + (zi*zi)) < four);
					vals.if_not_set(finished, vals + one);
					if (finished.all())
						break;
				}
				//Record values
				vals.store(&results[ix + (iy * RESOLUTION)]);
			}
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}

template<size_t RESOLUTION, size_t ITERATIONS>
void doMandelbrotIntrinsics(float* results) {
	auto start = my_clock::now();
	for (int I = 0; I < 100; I++) {
		alignas(32) float incr[] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };
		__m256 increment = _mm256_load_ps(incr);
		__m256 one = _mm256_set1_ps(1.0f);
		__m256 four = _mm256_set1_ps(4.0f);
		__m256 oneOverRes = _mm256_set1_ps(1.0f / RESOLUTION);
		for (int iy = 0; iy < RESOLUTION; iy++) {
			__m256 y = _mm256_mul_ps(_mm256_set1_ps((static_cast<float>(iy))), oneOverRes);
			for (int ix = 0; ix < RESOLUTION; ix += 8) {
				__m256 x = _mm256_mul_ps(_mm256_add_ps(increment, _mm256_set1_ps(ix)), oneOverRes);
				__m256 z = _mm256_setzero_ps();
				__m256 zi = _mm256_setzero_ps();
				__m256 vals = _mm256_setzero_ps();
				for (int i = 0; i < ITERATIONS; i++) {
					z = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(z, z), _mm256_mul_ps(zi, zi)), x);
					zi = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(2.0f), z), zi), y);
					__m256 finished = _mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(z, z), _mm256_mul_ps(zi, zi)), four, _CMP_LT_OS);
					vals = _mm256_blendv_ps(_mm256_add_ps(vals, one), vals, finished);
					if (_mm256_testc_ps(_mm256_cmp_ps(vals, vals, _CMP_EQ_UQ), finished) != 0)
						break;
				}
				//Record values
				_mm256_store_ps(&results[ix + (iy * RESOLUTION)], vals);
			}
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}

int _tmain(int argc, _TCHAR* argv[])
{
	using clock = my_clock;
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

	// Mandelbrot
	static const size_t RESOLUTION = 1024*2;
	static const size_t ITERATIONS = 100;
	std::unique_ptr<float[]> resultSeq = std::unique_ptr<float[]>(new float[RESOLUTION*RESOLUTION]);
	std::unique_ptr<float[]> resultVec = std::unique_ptr<float[]>(new float[RESOLUTION*RESOLUTION]);
	std::unique_ptr<float[]> resultInt = std::unique_ptr<float[]>(new float[RESOLUTION*RESOLUTION]);
	doMandelbrotFloat<RESOLUTION, ITERATIONS>(resultSeq.get());
	doMandelbrotVector<QuickVec::float8_avx, RESOLUTION, ITERATIONS>(resultVec.get());
	doMandelbrotIntrinsics<RESOLUTION, ITERATIONS>(resultInt.get());
	bool mismatched = false;
	for (size_t y = 0; y < RESOLUTION; y++) {
		for (size_t x = 0; x < RESOLUTION; x++) {
			size_t index = x + RESOLUTION*y;
			if (resultSeq[index] != resultVec[index]) {
				std::cout << "Mismatch at " << x << "," << y << " values were " << resultSeq[index] << " and " << resultInt[index] << std::endl;
				mismatched = true;
				break;
			}
		}
		if (mismatched) break;
	}


	std::cin.ignore();

	return 0;
}

