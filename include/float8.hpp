#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"

namespace QuickVec {
	namespace float8_avx_def {
		class bool8_avx;
		class float8_avx;

		using this_t = float8_avx;
		using bool_t = bool8_avx;

		class alignas(__m256) bool8_avx
		{
			// Only implicitly convert to and from raw in private
			bool_t(const __m256& o) : raw(o) {};
			operator __m256 () const { return raw; }
		public:
			static const size_t size = 8;
			__m256 raw;
			bool_t() = default;
			bool_t(bool_t&& o) = default;
			bool_t(const bool_t& o) = default;
			bool_t(const bool& o) {
				if (o) {
					raw = _mm256_cmp_ps(raw, raw, _CMP_EQ_UQ);
				}
				else {
					raw = _mm256_xor_ps(raw, raw);
				}
			}

			bool all() {
				return (_mm256_testc_ps(bool_t(true), raw) != 0);
			}

			friend class float8_avx;

			friend bool_t operator||(const bool_t& a, const bool_t& b);
			friend bool_t operator&&(const bool_t& a, const bool_t& b);

			friend bool_t operator<(const this_t& a, const this_t& b);
			friend bool_t operator<=(const this_t& a, const this_t& b);
			friend bool_t operator==(const this_t& a, const this_t& b);
			friend bool_t operator!=(const this_t& a, const this_t& b);
			friend bool_t operator>=(const this_t& a, const this_t& b);
			friend bool_t operator>(const this_t& a, const this_t& b);
		};

		class alignas(__m256) float8_avx
		{
			// Only implicitly convert to and from raw in private
			this_t(const __m256& o) : raw(o) {};
			operator __m256 () const { return raw; }
		public:
			using bool_t = bool_t;
			static const size_t size = 8;
			__m256 raw;
			__forceinline this_t() {
				raw = _mm256_setzero_ps();
			}
			__forceinline this_t(this_t&& o) = default;
			__forceinline this_t(const this_t& o) = default;
			__forceinline this_t& operator=(const this_t& o) = default;
			
			__forceinline this_t(float o) {
				raw = _mm256_set1_ps(o);
			}
			
			__forceinline this_t(float a, float b, float c, float d, float e, float f, float g, float h) {
				raw = _mm256_setr_ps(a, b, c, d, e, f, g, h);
			}

			static this_t loadAligned(const float* ptr) {
				return _mm256_load_ps(ptr);
			}

			static this_t load(const float* ptr) {
				return _mm256_loadu_ps(ptr);
			}

			void store(float* ptr) {
				_mm256_storeu_ps(ptr, raw);
			}

			this_t& if_set(const bool_t& mask, const this_t& newVal) {
				raw = _mm256_blendv_ps(raw, newVal, mask);
				return *this;
			}

			this_t& if_not_set(const bool_t& mask, const this_t& newVal) {
				raw = _mm256_blendv_ps(newVal, raw, mask);
				return *this;
			}

			friend this_t operator+(const this_t& a, const this_t& b);
			friend this_t operator-(const this_t& a, const this_t& b);
			friend this_t operator*(const this_t& a, const this_t& b);
			friend this_t operator/(const this_t& a, const this_t& b);

			this_t& operator+=(const this_t& o) { raw = *this + o; return *this; }
			this_t& operator-=(const this_t& o) { raw = *this - o; return *this; }
			this_t& operator*=(const this_t& o) { raw = *this * o; return *this; }
			this_t& operator/=(const this_t& o) { raw = *this / o; return *this; }

			friend bool_t operator<(const this_t& a, const this_t& b);
			friend bool_t operator<=(const this_t& a, const this_t& b);
			friend bool_t operator==(const this_t& a, const this_t& b);
			friend bool_t operator!=(const this_t& a, const this_t& b);
			friend bool_t operator>=(const this_t& a, const this_t& b);
			friend bool_t operator>(const this_t& a, const this_t& b);
		};

		using this_t = float8_avx;
		using bool_t = bool8_avx;

		/////////////////////////////////////////////////////////////
		// float8 arithmetic operators
		/////////////////////////////////////////////////////////////
		inline this_t operator+(const this_t& a, const this_t& b) {
			return _mm256_add_ps(a, b);
		}

		inline this_t operator-(const this_t& a, const this_t& b) {
			return _mm256_sub_ps(a, b);
		}

		inline this_t operator*(const this_t& a, const this_t& b) {
			return _mm256_mul_ps(a, b);
		}

		inline this_t operator/(const this_t& a, const this_t& b) {
			return _mm256_div_ps(a, b);
		}

		/////////////////////////////////////////////////////////////
		// float8 comparison operators
		/////////////////////////////////////////////////////////////
		inline bool_t operator<(const this_t& a, const this_t& b) {
			return _mm256_cmp_ps(a, b, _CMP_LT_OS);
		}

		inline bool_t operator<=(const this_t& a, const this_t& b) {
			return _mm256_cmp_ps(a, b, _CMP_LE_OS);
		}

		inline bool_t operator==(const this_t& a, const this_t& b) {
			return _mm256_cmp_ps(a, b, _CMP_EQ_OS);
		}

		inline bool_t operator!=(const this_t& a, const this_t& b) {
			return _mm256_cmp_ps(a, b, _CMP_NEQ_OS);
		}

		inline bool_t operator>=(const this_t& a, const this_t& b) {
			return _mm256_cmp_ps(a, b, _CMP_GE_OS);
		}

		inline bool_t operator>(const this_t& a, const this_t& b) {
			return _mm256_cmp_ps(a, b, _CMP_GT_OS);
		}

		/////////////////////////////////////////////////////////////
		// bool8 binary operators
		/////////////////////////////////////////////////////////////
		inline bool_t operator||(const bool_t& a, const bool_t& b) {
			return _mm256_or_ps(a, b);
		}
		inline bool_t operator&&(const bool_t& a, const bool_t& b) {
			return _mm256_and_ps(a, b);
		}
	}
	using float8_avx_def::float8_avx;
}