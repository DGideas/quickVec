#include <tchar.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <iomanip>

#include "QuickVec.hpp"
#include "cpu_support.hpp"

//#include "float8.hpp"
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

enum SIMDType {
	None,
	SSE,
	SSE2,
	SSE3,
	SSE4_1,
	SSE4_2,
	AVX,
	AVX2,
	AVX_512
};

SIMDType selectFastestVector() {
	InstructionSet cpu_info;
	if (cpu_info.AVX512F()) {
		return AVX_512;
	}
	if (cpu_info.AVX2()) {
		return AVX2;
	}
	if (cpu_info.AVX2()) {
		return AVX;
	}
	if (cpu_info.SSE42()) {
		return SSE4_2;
	}
	if (cpu_info.SSE41()) {
		return SSE4_1;
	}
	if (cpu_info.SSE3()) {
		return SSE3;
	}
	if (cpu_info.SSE2()) {
		return SSE2;
	}
	if (cpu_info.SSE()) {
		return SSE;
	}
	return None;
}

using namespace QuickVec;
template<typename function_t, typename... Args>
void runWithFloatType(SIMDType type, function_t func, Args&&... args) {
	switch (type) {
	case AVX_512:
	case AVX2:
	case AVX:
		func.run<float8_avx>(args...);
		break;
	case SSE4_2:
	case SSE4_1:
		func.run<float4_sse4_1>(args...);
		break;
	case SSE3:
	case SSE2:
		func.run<float4_sse2>(args...);
		break;
	case SSE:
		func.run<float4_sse>(args...);
		break;
	case None:
		//func.run<float_base<4>>(args...);
		break;
	}
}

template<size_t RESOLUTION, size_t ITERATIONS>
void runMandelbrotAuto(float* results) {
	SIMDType type = selectFastestVector();
	runWithFloatType(type, mandelbrot_functor<RESOLUTION, ITERATIONS>(), results);
}

using time_point = std::chrono::time_point<std::chrono::system_clock>;
using time_diff = std::chrono::duration<double>;
using my_clock = std::chrono::system_clock;

template<size_t RESOLUTION, size_t ITERATIONS>
struct mandelbrot_functor {
	template<typename float_vec>
	void run(float* results) {
		auto start = my_clock::now();
		float incr[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		float_vec increment = float_vec::load(incr);
		float oneOverRes = 1.0f / RESOLUTION;
		for (int iy = 0; iy < RESOLUTION; iy++) {
			float_vec y = float_vec(static_cast<float>(iy) * oneOverRes);
			for (int ix = 0; ix < RESOLUTION; ix += float_vec::size) {
				float_vec x = (increment + ix) * oneOverRes;
				float_vec z, zi, vals;
				z = zi = vals = float_vec::zero();
				float_vec::bool_t finished(false);
				for (int i = 0; i < ITERATIONS; i++) {
					float_vec a = z*z;
					float_vec b = zi*zi;
					finished |= ((a + b) > 4.0f);
					if (finished.all())
						break;
					vals.if_not_set(finished, vals + 1.0f);
					zi = (2.0f*z*zi) + y;
					z = a - b + x;
				}
				//Record values
				vals.store(&results[ix + (iy * RESOLUTION)]);
			}
		}
		auto end = my_clock::now();
		time_diff duration = end - start;
		std::cout << "Mandelbrot took " << duration.count() << "s\n";
	}
};

template<size_t RESOLUTION, size_t ITERATIONS>
void doMandelbrotFloat(float* results) {
	auto start = my_clock::now();
	float oneOverRes = 1.0f / RESOLUTION;
	for (int iy = 0; iy < RESOLUTION; iy++) {
		float y = static_cast<float>(iy) *oneOverRes;
		for (int ix = 0; ix < RESOLUTION; ix++) {
			float x = static_cast<float>(ix) * oneOverRes;
			float z = 0.0f;
			float zi = 0.0f;
			float val = 0.0f;
			for (int i = 0; i < ITERATIONS; i++) {
				float a = z*z;
				float b = zi*zi;
				if (a + b > 4.0f) {
					break;
				}
				val++;
				zi = (2.0f*z*zi) + y;
				z = a - b + x;
			}
			//Record values
			results[ix + (iy * RESOLUTION)] = val;
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}

template<typename float_vec, size_t RESOLUTION, size_t ITERATIONS>
void doMandelbrotVector(float* results) {
	auto start = my_clock::now();
	float incr[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
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
				float_vec::bool_t finished = ((a + b) > four);
				if (finished.all())
					break;
				vals.if_not_set(finished, vals + one);
				zi = (2.0f*z*zi) + y;
				z = a - b + x;
			}
			//Record values
			vals.store(&results[ix + (iy * RESOLUTION)]);
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}

template<size_t RESOLUTION, size_t ITERATIONS>
void doMandelbrotIntrinsics(float* results) {
	auto start = my_clock::now();
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
			__m256 finished = _mm256_setzero_ps();
			for (int i = 0; i < ITERATIONS; i++) {
				// a= z*z;
				__m256 a = _mm256_mul_ps(z, z);
				// b= zi*zi;
				__m256 b = _mm256_mul_ps(zi, zi);
				// finished = finished | (a+b > 4)
				finished = _mm256_or_ps(finished, _mm256_cmp_ps(_mm256_add_ps(a,b), four, _CMP_GT_OQ));
				// if (finished) break;
				if (_mm256_testc_ps(_mm256_cmp_ps(vals, vals, _CMP_EQ_UQ), finished) == 0)
					break;
				// vals = vals + 1;
				vals = _mm256_blendv_ps(_mm256_add_ps(vals, one), vals, finished);
				// zi = 2.0f*z*zi + y;
				zi = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(2.0f), z), zi), y);
				// z = a - b + x;
				z = _mm256_add_ps(_mm256_sub_ps(a, b), x);
			}
			//Record values
			_mm256_storeu_ps(&results[ix + (iy * RESOLUTION)], vals);
		}
	}
	auto end = my_clock::now();
	time_diff duration = end - start;
	std::cout << "Mandelbrot took " << duration.count() << "s\n";
}

template <size_t RESOLUTION>
void printSet(float* set) {
	for (size_t y = 0; y < RESOLUTION; y++) {
		for (size_t x = 0; x < RESOLUTION; x++) {
			size_t index = x + RESOLUTION*y;
			std::cout << (int)set[index];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Mandelbrot
	static const size_t RESOLUTION = 1024*4;
	static const size_t ITERATIONS = 50;
	std::unique_ptr<float[]> resultSeq = std::unique_ptr<float[]>(new float[RESOLUTION*RESOLUTION]);
	std::unique_ptr<float[]> resultVec = std::unique_ptr<float[]>(new float[RESOLUTION*RESOLUTION]);
	std::unique_ptr<float[]> resultInt = std::unique_ptr<float[]>(new float[RESOLUTION*RESOLUTION]);
	std::cout << "Press q to quit. Press any other key to recompute" << std::endl;
	while (true) {
		doMandelbrotFloat<RESOLUTION, ITERATIONS>(resultSeq.get());
		runMandelbrotAuto<RESOLUTION, ITERATIONS>(resultVec.get());
		doMandelbrotIntrinsics<RESOLUTION, ITERATIONS>(resultInt.get());
		bool mismatched = false;
		for (size_t y = 0; y < RESOLUTION; y++) {
			for (size_t x = 0; x < RESOLUTION; x++) {
				size_t index = x + RESOLUTION*y;
				if (resultVec[index] != resultSeq[index]) {
					std::cout << "Mismatch in QuickVec implementation at " << x << "," << y << ". Expected " << resultSeq[index] << ", but got" << resultVec[index] << std::endl;
					mismatched = true;
					break;
				}
				if (resultInt[index] != resultSeq[index]) {
					std::cout << "Mismatch in intrinsic implementation at " << x << "," << y << ". Expected " << resultSeq[index] << ", but got " << resultInt[index] << std::endl;
					mismatched = true;
					break;
				}
				//std::cout << resultSeq[index];
			}
			//std::cout << std::endl;
			if (mismatched) break;
		}
		if (!mismatched) {
			std::cout << "!!! Results match sequential! XD Test Passed !!!" << std::endl;
		}
		else {
			std::cout << "XXXX - An error occured! TEST FAILED!!!! - XXXX" << std::endl;
			//printSet<RESOLUTION>(resultSeq.get());
			//printSet<RESOLUTION>(resultVec.get());
		}

		// loop on key, but quit on q
		char c;
		std::cin.get(c);
		if (c == 'Q' || c == 'q') {
			break;
		}
	}

	return 0;
}

