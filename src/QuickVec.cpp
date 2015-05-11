#include <tchar.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <iomanip>

#include "QuickVec.hpp"
#include "float8.hpp"
//#include "numeric_vec.hpp"
//#include "bool_vec.hpp"
//#include "float_vec.hpp"
///////////////////////////////////////////////

#if defined(__arm__) || \
	defined(__TARGET_ARCH_ARM) || \
	defined(_ARM) || \
	defined(_M_ARM) || \
	defined(__arm)
#define ARM_MODE
#endif




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
				int i;
				for (i = 0; i < ITERATIONS; i++) {
					float a = z*z;
					float b = zi*zi;
					if (a + b > 4.0f) {
						break;
					}
					zi = (2.0f*z*zi) + y;
					z = a - b + x;
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
		float incr[] = {0, 1, 2, 3, 4, 5, 6, 7};
		float_vec increment = float_vec::load(incr);
		float_vec one = 1.0f;
		float_vec four = 4.0f;
		float_vec oneOverRes = 1.0f / RESOLUTION;
		for (int iy = 0; iy < RESOLUTION; iy++) {
			float_vec y = float_vec(static_cast<float>(iy)) * oneOverRes;
			for (int ix = 0; ix < RESOLUTION; ix += float_vec::size) {
				float_vec x = (increment + ix) * oneOverRes;
				float_vec z, zi, vals;
				z = zi = vals = float_vec::zero();
				for (int i = 0; i < ITERATIONS; i++) {
					float_vec a = z*z;
					float_vec b = zi*zi;
					float_vec::bool_t finished = ((a+b) > four);
					if (finished.all())
						break;
					vals.if_not_set(finished, vals + one);
					zi = 2.0f*z*zi + y;
					z = a - b + x;
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
					__m256 finished = _mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(z, z), _mm256_mul_ps(zi, zi)), four, _CMP_GT_OQ);
					if (_mm256_testc_ps(_mm256_cmp_ps(vals, vals, _CMP_EQ_UQ), finished) == 0)
						break;
					vals = _mm256_blendv_ps(_mm256_add_ps(vals, one), vals, finished);
					__m256 a = _mm256_mul_ps(z, z);
					__m256 b = _mm256_mul_ps(zi, zi);
					zi = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(2.0f), z), zi), y);
					z = _mm256_add_ps(_mm256_sub_ps(a, b), x);
				}
				//Record values
				_mm256_storeu_ps(&results[ix + (iy * RESOLUTION)], vals);
			}
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}
/*
enum InstructionSupport{
#ifndef ARM
	None,
	SSE,
	SSE2,
	SSE3,
	SSE4_1,
	SSE4_2,
	AVX,
	AVX2,
	AVX_512
}

doMandelbrotSelect(int support) {
	switch support

}*/

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
		fp v = 0.0f;
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
	static const size_t RESOLUTION = 32;
	static const size_t ITERATIONS = 50;
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
			if (resultVec[index] != resultInt[index]) {
				std::cout << "Mismatch at " << x << "," << y << " values were " << resultSeq[index] << " and " << resultVec[index] << std::endl;
				mismatched = true;
				break;
			}
			//std::cout << resultSeq[index];
		}
		//std::cout << std::endl;
		if (mismatched) break;
	}
	std::cout << std::endl;
	for (size_t y = 0; y < RESOLUTION; y++) {
		for (size_t x = 0; x < RESOLUTION; x++) {
			size_t index = x + RESOLUTION*y;
			std::cout << std::setfill('0') << std::setw(2) << resultSeq[index] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	for (size_t y = 0; y < RESOLUTION; y++) {
		for (size_t x = 0; x < RESOLUTION; x++) {
			size_t index = x + RESOLUTION*y;
			std::cout << std::setfill('0') << std::setw(2) << resultVec[index] << " ";
		}
		std::cout << std::endl;
	}

	std::cin.ignore();

	return 0;
}

