#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"
namespace QuickVec {

	struct m256_float {
		__m256 raw;
		m256_float() = default;
		m256_float(const __m256& o) : raw(o) {};
		operator const __m256 () const { return raw; }
		operator __m256& () { return raw; }
		const float& operator[](size_t i) const { return raw.m256_f32[i]; };
		float& operator[](size_t i) { return raw.m256_f32[i]; };
	};

	class bool8_avx {
		using this_t = bool8_avx;
		using T = int;
		using Data_t = __m256;
		static const size_t N = 8;
	public:
		Data_t data;

		this_t() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		this_t(T&& t, Args&&... args) :data({ t, static_cast<T>(args)... }) {};

		template <typename Enable = std::enable_if < (N>1)>::type >
		this_t(T&& t) {
			data = _mm256_castsi256_ps(_mm256_set1_epi32(t));
		}

		this_t(Data_t&& d) : data(d) {};

		bool all() const {
			__m256 junk = _mm256_setzero_ps();
			return (_mm256_testc_ps(_mm256_cmp_ps(junk, junk, _CMP_EQ_UQ), data) == 0);
		}

		bool any() const {
			return (_mm256_testz_ps(data, data) == 0);
		}

		//[]
		T& operator[](size_t i) { return reinterpret_cast<int&>(data.m256_f32[i]); }
		T operator[](size_t i) const { return  reinterpret_cast<const int&>(data.m256_f32[i]); }

		this_t operator&&(const this_t& o) const { return this_t(_mm256_and_ps(data, o.data)); }
		this_t operator||(const this_t& o) const { return this_t(_mm256_or_ps(data, o.data)); }
		this_t& operator&=(const this_t& o) { data = (*this && o).data; return *this; }
		this_t& operator|=(const this_t& o) { data = (*this || o).data; return *this; }
	};

	class alignas(__m256) float8_avx : public float_base<8, m256_float>{
	protected:
		static const uint32_t N = 8;
		using T = float;
		using data_t = m256_float;
		using this_t = float8_avx;
		using base_t = float_base<8, data_t>;

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m256 allOnes() { return _mm256_cmp_ps(_mm256_setzero_ps(), _mm256_setzero_ps(), _CMP_EQ_UQ); }

	public:
		using bool_t = bool8_avx;

		this_t() = default;

		this_t(const base_t& o) : base_t(o) {};

		this_t(float a, float b, float c, float d, float e, float f, float g, float h) {
			data = _mm256_setr_ps(a, b, c, d, e, f, g, h);
		}

		this_t(float a) {
			data = _mm256_set1_ps(a);
		}

		this_t(const data_t& d) {
			data = d;
		}

		this_t(float* ptr, bool aligned = false) {
			if (aligned) {
				data = loadAligned(ptr).data;
			}
			else {
				data = load(ptr).data;
			}
		}

		static this_t zero() {
			return this_t(_mm256_setzero_ps());
		}

		static this_t loadAligned(float* ptr) {
			return this_t(_mm256_load_ps(ptr));
		}

		static this_t load(float* ptr) {
			return this_t(_mm256_loadu_ps(ptr));
		}

		this_t& store(float* ptr) {
			_mm256_storeu_ps(ptr, data);
			return *this;
		}

		this_t& storeAligned(float* ptr) {
			_mm256_store_ps(ptr, data);
			return *this;
		}

		this_t& if_set(const bool_t& mask, const this_t& newVal) {
			data = _mm256_blendv_ps(data, newVal.data, mask.data);
			return *this;
		}

		this_t& if_not_set(const bool_t& mask, const this_t& newVal) {
			data = _mm256_blendv_ps(newVal.data, data, mask.data);
			return *this;
		}

		//Binary ops
		//+ - * / % | & ^ >> <<
		this_t& operator+=(const this_t& o) { data = (*this + o).data; return *this; }
		this_t& operator-=(const this_t& o) { data = (*this - o).data; return *this; }
		this_t& operator*=(const this_t& o) { data = (*this * o).data; return *this; }
		this_t& operator/=(const this_t& o) { data = (*this / o).data; return *this; }
		//this_t& operator%=(const this_t& o) { data = (*this % o).data; return *this; }

		this_t operator+(const this_t& o) const { return _mm256_add_ps(data, o.data); }
		this_t operator-(const this_t& o) const { return _mm256_sub_ps(data, o.data); }
		this_t operator*(const this_t& o) const { return _mm256_mul_ps(data, o.data); }
		this_t operator/(const this_t& o) const { return _mm256_div_ps(data, o.data); }

		//this_t& operator%=(const this_t& o) { }
		//this_t operator%(const this_t& o) const {}

		//Unary ops
		//-
		this_t operator-() const { return this_t(_mm256_setzero_ps()) - *this; }

		//Comparisons
		//== != < <= > >=
		bool_t operator<(const this_t& o) const { return bool_t(_mm256_cmp_ps(data, o.data, _CMP_LT_OS)); }
		bool_t operator<=(const this_t& o) const { return bool_t(_mm256_cmp_ps(data, o.data, _CMP_LE_OS)); }
		bool_t operator==(const this_t& o) const { return bool_t(_mm256_cmp_ps(data, o.data, _CMP_EQ_OS)); }
		bool_t operator!=(const this_t& o) const { return bool_t(_mm256_cmp_ps(data, o.data, _CMP_NEQ_OS)); }
		bool_t operator>=(const this_t& o) const { return bool_t(_mm256_cmp_ps(data, o.data, _CMP_GE_OS)); }
		bool_t operator>(const this_t& o) const { return bool_t(_mm256_cmp_ps(data, o.data, _CMP_GT_OS)); }

		//[]
		//inherit
	};

	inline float8_avx operator+(const float& a, const float8_avx& b) {
		return b + a;
	}

	inline float8_avx operator-(const float& a, const float8_avx& b) {
		return float8_avx(a) - b;
	}

	inline float8_avx operator*(const float& a, const float8_avx& b) {
		return b * a;
	}

	inline float8_avx operator/(const float& a, const float8_avx& b) {
		return float8_avx(a) / b;
	}
}