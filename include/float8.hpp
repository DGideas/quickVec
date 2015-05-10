#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"

namespace QuickVec {
	namespace float8_avx_def {
		class bool8_avx;
		class float8_avx;

		using this_t = float8_avx;
		using bool_t = bool8_avx;
		using data_t = __m256;

		class alignas(data_t) bool8_avx
		{
			// Only implicitly convert to and from raw in private
			bool_t(const data_t& o) : raw(o) {};
			operator data_t () const { return raw; }
		public:
			static const size_t size = 8;
			data_t raw;
			bool_t() = default;
			bool_t(bool_t&& o) = default;
			bool_t(const bool_t& o) = default;
			bool_t(const bool& o);

			bool all() const;

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

		inline bool_t::bool8_avx(const bool& o) {
			if (o) {
				raw = _mm256_cmp_ps(raw, raw, _CMP_EQ_UQ);
			}
			else {
				raw = _mm256_xor_ps(raw, raw);
			}
		}

		inline bool bool_t::all() const {
			return (_mm256_testc_ps(bool_t(true), raw) != 0);
		}

		inline bool all(const bool_t& b) {
			return b.all();
		}

		class alignas(data_t) float8_avx
		{
			// Only implicitly convert to and from raw in private
			this_t(const data_t& o) : raw(o) {};
			operator data_t () const { return raw; }
		public:
			using bool_t = bool_t;
			static const size_t size = 8;
			data_t raw;
			this_t();
			this_t(this_t&& o) = default;
			this_t(const this_t& o) = default;
			this_t& operator=(const this_t& o) = default;
			
			this_t(float o);
			this_t(const float* ptr, bool aligned = false) {
				if (aligned) {
					raw = loadAligned(ptr);
				}
				else {
					raw = load(ptr);
				}
			}

			static this_t loadAligned(const float* ptr);
			static this_t load(const float* ptr);

			void store(float* ptr);
			void storeAligned(float* ptr);

			this_t& if_set(const bool_t& mask, const this_t& newVal);
			this_t& if_not_set(const bool_t& mask, const this_t& newVal);

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

		inline this_t::float8_avx() {
			raw = _mm256_setzero_ps();
		}

		inline this_t::float8_avx(float o) {
			raw = _mm256_set1_ps(o);
		}

		inline this_t this_t::loadAligned(const float* ptr) {
			return _mm256_load_ps(ptr);
		}

		inline this_t this_t::load(const float* ptr) {
			return _mm256_loadu_ps(ptr);
		}

		inline void this_t::store(float* ptr) {
			_mm256_storeu_ps(ptr, raw);
		}
		
		inline void this_t::storeAligned(float* ptr) {
			_mm256_storeu_ps(ptr, raw);
		}

		this_t& this_t::if_set(const bool_t& mask, const this_t& newVal) {
			raw = _mm256_blendv_ps(raw, newVal, mask);
			return *this;
		}

		this_t& this_t::if_not_set(const bool_t& mask, const this_t& newVal) {
			raw = _mm256_blendv_ps(newVal, raw, mask);
			return *this;
		}

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