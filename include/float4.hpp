#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"
namespace QuickVec {

	struct m128_float {
		__m128 raw;
		m128_float() = default;
		m128_float(const __m128& o) : raw(o) {};
		operator const __m128 () const { return raw; }
		operator __m128& () { return raw; }
		const float& operator[](size_t i) const { return raw.m128_f32[i]; };
		float& operator[](size_t i) { return raw.m128_f32[i]; };
	};
	
	class bool4_sse {
		using this_t = bool4_sse;
		using T = int;
		using Data_t = __m128;
		static const size_t N = 4;
	public:
		Data_t data;

		this_t() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		this_t(T&& t, Args&&... args) :data({ t, static_cast<T>(args)... }) {};

		template <typename Enable = std::enable_if<(N>1)>::type>
		this_t(T&& t) {
			data = _mm_castsi128_ps(_mm_set1_epi32(t));
		}

		this_t(Data_t&& d) : data(d) {};

		bool all() const {
			__m128 junk = _mm_setzero_ps();
			return (_mm_testc_ps(_mm_cmpeq_ps(junk,junk), data) == 0);
		}

		bool any() const {
			return (_mm_testz_ps(data, data) == 0);
		}

		//[]
		T& operator[](size_t i) { return reinterpret_cast<int&>(data.m128_f32[i]); }
		T operator[](size_t i) const { return  reinterpret_cast<const int&>(data.m128_f32[i]); }

		this_t operator&&(const this_t& o) const { return this_t(_mm_and_ps(data, o.data)); }
		this_t operator||(const this_t& o) const { return this_t(_mm_or_ps(data, o.data)); }
		this_t& operator&=(const this_t& o) { data = (*this && o).data; return *this; }
		this_t& operator|=(const this_t& o) { data = (*this || o).data; return *this; }
	};

	class alignas(__m128) float4_sse : public float_base<4, m128_float> {
	protected:
		static const uint32_t N = 4;
		using T = float;
		using data_t = m128_float;
		using this_t = float4_sse;
		using base_t = float_base<4, data_t>;

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m128 allOnes() { return _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()); }

	public:
		using bool_t = bool4_sse;

		this_t() = default;

		this_t(const base_t& o) : base_t(o) {};

		this_t(float a, float b, float c, float d){
			data = _mm_setr_ps(a, b, c, d);
		}

		this_t(float a) {
			data = _mm_set_ps1(a);
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
			return this_t(_mm_setzero_ps());
		}

		static this_t loadAligned(float* ptr) {
			return this_t(_mm_load_ps(ptr));
		}

		static this_t load(float* ptr) {
			return this_t(_mm_loadu_ps(ptr));
		}

		this_t& store(float* ptr) {
			_mm_storeu_ps(ptr, data);
			return *this;
		}

		this_t& storeAligned(float* ptr) {
			_mm_store_ps(ptr, data);
			return *this;
		}

		this_t& if_set(const bool_t& mask, const this_t& newVal) {
			__m128 maskA = _mm_andnot_ps(mask.data, data);
			__m128 maskB = _mm_and_ps(mask.data, newVal.data);
			data = _mm_or_ps(maskA, maskB);
			return *this;
		}

		this_t& if_not_set(const bool_t& mask, const this_t& newVal) {
			__m128 maskA = _mm_and_ps(mask.data,data);
			__m128 maskB = _mm_andnot_ps(mask.data, newVal.data);
			data = _mm_or_ps(maskA, maskB);
			return *this;
		}

		//Binary ops
		//+ - * / % | & ^ >> <<
		this_t& operator+=(const this_t& o) { data = (*this + o).data; return *this; }
		this_t& operator-=(const this_t& o) { data = (*this - o).data; return *this; }
		this_t& operator*=(const this_t& o) { data = (*this * o).data; return *this; }
		this_t& operator/=(const this_t& o) { data = (*this / o).data; return *this; }

		this_t operator+(const this_t& o) const { return _mm_add_ps(data, o.data); }
		this_t operator-(const this_t& o) const { return _mm_sub_ps(data, o.data); }
		this_t operator*(const this_t& o) const { return _mm_mul_ps(data, o.data); }
		this_t operator/(const this_t& o) const { return _mm_div_ps(data, o.data); }

		//this_t& operator%=(const this_t& o) { }
		//this_t operator%(const this_t& o) const {}

		this_t& operator|=(const this_t& o) { data = _mm_or_ps(data, o.data); return *this; }
		this_t operator|(const this_t& o) const { return _mm_or_ps(data, o.data); }

		this_t& operator&=(const this_t& o) { data = _mm_and_ps(data, o.data); return *this; }
		this_t operator&(const this_t& o) const { return _mm_and_ps(data, o.data); }
		
		this_t& operator^=(const this_t& o) { data = _mm_xor_ps(data, o.data); return *this; }
		this_t operator^(const this_t& o) const { return _mm_xor_ps(data, o.data); }

		//Unary ops
		//-~!
		this_t operator-() const { return this_t(_mm_setzero_ps()) - *this; }
		this_t operator~() const { return _mm_xor_ps(allOnes(), data); }
		this_t operator!() const { return _mm_cmpeq_ps(_mm_setzero_ps(), data); }

		//Comparisons
		//== != < <= > >=
		bool_t operator<(const this_t& o) const { return bool_t(_mm_cmplt_ps(data, o.data)); }
		bool_t operator>(const this_t& o) const { return bool_t(_mm_cmpgt_ps(data, o.data)); }

		//[]
	};

	inline float4_sse operator+(const float& a, const float4_sse& b) {
		return b + a;
	}

	inline float4_sse operator-(const float& a, const float4_sse& b) {
		return float4_sse(a) - b;
	}

	inline float4_sse operator*(const float& a, const float4_sse& b) {
		return b + a;
	}

	inline float4_sse operator/(const float& a, const float4_sse& b) {
		return float4_sse(a) / b;
	}

	class float4_sse2 : public float4_sse {
	protected:
		using this_t = float4_sse2;
		using base_t = float4_sse;
	public:
		//forward constructors
		using base_t::base_t;

		this_t& operator%=(const this_t& o) {
			// a - b * floor(a/b)
			data = (*this % o).data;
			return *this;
		}
		this_t operator%(const this_t& o) const {
			this_t ret(_mm_sub_ps(data, _mm_mul_ps(o.data, _mm_cvtepi32_ps(_mm_cvtps_epi32(_mm_div_ps(data, o.data))))));
			__m128 mask = _mm_cmplt_ps(ret.data, _mm_setzero_ps());
			ret.data = _mm_add_ps(ret.data, _mm_and_ps(mask, _mm_andnot_ps(_mm_set1_ps(-0.0f), o.data)));
			return ret;
		}
	};

	class float4_sse4_1 : public float4_sse2 {
		using this_t = float4_sse4_1;
		using base_t = float4_sse2;
	public:
		// Forward constructors
		using base_t::base_t;

		this_t& operator%=(const this_t& o) {
			// a - b * floor(a/b)
			data = (*this % o).data;
			return *this;
		}

		this_t operator%(const this_t& o) const {
			this_t ret(_mm_sub_ps(data, _mm_mul_ps(o.data, _mm_floor_ps(_mm_div_ps(data, o.data)))));
			__m128 mask = _mm_cmplt_ps(ret.data, _mm_setzero_ps());
			ret.data = _mm_add_ps(ret.data, _mm_and_ps(mask, _mm_andnot_ps(_mm_set1_ps(-0.0f), o.data)));
			return ret;
		}
	};


}