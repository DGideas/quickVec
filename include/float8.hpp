#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"

namespace QuickVec {
	namespace bool_vec_def {
		template<typename data_t, size_t size>
		class alignas(data_t)bool_vec
		{
			using bool_t = bool_vec<data_t, size>;
			// Only implicitly convert to and from raw in private
			//bool_t(const data_t& o) : raw(o) {};
			operator data_t () const { return raw; }
		public:
			static const size_t size = size;
			data_t raw;
			bool_t() = default;
			bool_t(bool_t&& o) = default;
			bool_t(const bool_t& o) = default;
			bool_t(const bool& o);
			explicit bool_t(const data_t& o) : raw(o) {};

			bool all() const;

			friend class float8_avx;

			friend bool_t operator||(const bool_t& a, const bool_t& b);
			friend bool_t operator&&(const bool_t& a, const bool_t& b);

			/*template<typename vec_t>
			friend bool_t operator<(const vec_t& a, const vec_t& b);
			template<typename vec_t>
			friend bool_t operator<=(const vec_t& a, const vec_t& b);
			template<typename vec_t>
			friend bool_t operator==(const vec_t& a, const vec_t& b);
			template<typename vec_t>
			friend bool_t operator!=(const vec_t& a, const vec_t& b);
			template<typename vec_t>
			friend bool_t operator>=(const vec_t& a, const vec_t& b);
			template<typename vec_t>
			friend bool_t operator>(const vec_t& a, const vec_t& b);*/
		};
	}

	////////////////////////////////////////////////////////////////
	// bool avx file
	namespace bool_vec_def {
		using data_t = __m256;
		static const size_t size = 8;
		using bool_t = bool_vec<data_t, size>;

		template<>
		inline bool_t::bool_vec(const bool& o) {
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

		/////////////////////////////////////////////////////////////
		// bool8 binary operators
		/////////////////////////////////////////////////////////////
		inline bool_t operator||(const bool_t& a, const bool_t& b) {
			return bool_t(_mm256_or_ps(a, b));
		}
		inline bool_t operator&&(const bool_t& a, const bool_t& b) {
			return bool_t(_mm256_and_ps(a, b));
		}
	}
	using bool8_avx = bool_vec_def::bool_vec<__m256, 8>;



	namespace float8_avx_def {

		class float8_avx;

		using bool_t = bool8_avx;
		using vec_t = float8_avx;
		using data_t = __m256;
		static const size_t size = 8;
		

		

		

		class alignas(data_t) float8_avx
		{
			// Only implicitly convert to and from raw in private
			vec_t(const data_t& o) : raw(o) {};
			operator data_t () const { return raw; }
		public:
			using bool_t = bool_t;
			static const size_t size = 8;
			data_t raw;
			vec_t();
			vec_t(vec_t&& o) = default;
			vec_t(const vec_t& o) = default;
			vec_t& operator=(const vec_t& o) = default;
			
			vec_t(float o);
			vec_t(const float* ptr, bool aligned = false) {
				if (aligned) {
					raw = loadAligned(ptr);
				}
				else {
					raw = load(ptr);
				}
			}

			static vec_t loadAligned(const float* ptr);
			static vec_t load(const float* ptr);

			void store(float* ptr);
			void storeAligned(float* ptr);

			vec_t& if_set(const bool_t& mask, const vec_t& newVal);
			vec_t& if_not_set(const bool_t& mask, const vec_t& newVal);

			friend vec_t operator+(const vec_t& a, const vec_t& b);
			friend vec_t operator-(const vec_t& a, const vec_t& b);
			friend vec_t operator*(const vec_t& a, const vec_t& b);
			friend vec_t operator/(const vec_t& a, const vec_t& b);

			vec_t& operator+=(const vec_t& o) { raw = *this + o; return *this; }
			vec_t& operator-=(const vec_t& o) { raw = *this - o; return *this; }
			vec_t& operator*=(const vec_t& o) { raw = *this * o; return *this; }
			vec_t& operator/=(const vec_t& o) { raw = *this / o; return *this; }

			friend bool_t operator<(const vec_t& a, const vec_t& b);
			friend bool_t operator<=(const vec_t& a, const vec_t& b);
			friend bool_t operator==(const vec_t& a, const vec_t& b);
			friend bool_t operator!=(const vec_t& a, const vec_t& b);
			friend bool_t operator>=(const vec_t& a, const vec_t& b);
			friend bool_t operator>(const vec_t& a, const vec_t& b);
		};

		inline vec_t::float8_avx() {
			raw = _mm256_setzero_ps();
		}

		inline vec_t::float8_avx(float o) {
			raw = _mm256_set1_ps(o);
		}

		inline vec_t vec_t::loadAligned(const float* ptr) {
			return _mm256_load_ps(ptr);
		}

		inline vec_t vec_t::load(const float* ptr) {
			return _mm256_loadu_ps(ptr);
		}

		inline void vec_t::store(float* ptr) {
			_mm256_storeu_ps(ptr, raw);
		}
		
		inline void vec_t::storeAligned(float* ptr) {
			_mm256_storeu_ps(ptr, raw);
		}

		vec_t& vec_t::if_set(const bool_t& mask, const vec_t& newVal) {
			raw = _mm256_blendv_ps(raw, newVal, mask.raw);
			return *this;
		}

		vec_t& vec_t::if_not_set(const bool_t& mask, const vec_t& newVal) {
			raw = _mm256_blendv_ps(newVal, raw, mask.raw);
			return *this;
		}

		/////////////////////////////////////////////////////////////
		// float8 arithmetic operators
		/////////////////////////////////////////////////////////////
		inline vec_t operator+(const vec_t& a, const vec_t& b) {
			return _mm256_add_ps(a, b);
		}

		inline vec_t operator-(const vec_t& a, const vec_t& b) {
			return _mm256_sub_ps(a, b);
		}

		inline vec_t operator*(const vec_t& a, const vec_t& b) {
			return _mm256_mul_ps(a, b);
		}

		inline vec_t operator/(const vec_t& a, const vec_t& b) {
			return _mm256_div_ps(a, b);
		}

		/////////////////////////////////////////////////////////////
		// float8 comparison operators
		/////////////////////////////////////////////////////////////
		inline bool_t operator<(const vec_t& a, const vec_t& b) {
			return bool_t(_mm256_cmp_ps(a, b, _CMP_LT_OS));
		}

		inline bool_t operator<=(const vec_t& a, const vec_t& b) {
			return bool_t(_mm256_cmp_ps(a, b, _CMP_LE_OS));
		}

		inline bool_t operator==(const vec_t& a, const vec_t& b) {
			return bool_t(_mm256_cmp_ps(a, b, _CMP_EQ_OS));
		}

		inline bool_t operator!=(const vec_t& a, const vec_t& b) {
			return bool_t(_mm256_cmp_ps(a, b, _CMP_NEQ_OS));
		}

		inline bool_t operator>=(const vec_t& a, const vec_t& b) {
			return bool_t(_mm256_cmp_ps(a, b, _CMP_GE_OS));
		}

		inline bool_t operator>(const vec_t& a, const vec_t& b) {
			return bool_t(_mm256_cmp_ps(a, b, _CMP_GT_OS));
		}

		
	}
	using float8_avx_def::float8_avx;
}