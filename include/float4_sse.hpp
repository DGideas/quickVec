#pragma once

#include <cassert>
#include <xmmintrin.h>
#include "float_base.hpp"
namespace QuickVec {

	struct m128_accessor {
		template<size_t I>
		static float& get(__m128& data) {
			return data.m128_f32[I];
		}

		template<size_t I>
		static float get(const __m128& data) {
			return data.m128_f32[I];
		}

		static float& get(__m128& data, size_t I) {
			return data.m128_f32[I];
		}

		static float get(const __m128& data, size_t I) {
			return data.m128_f32[I];
		}
	};

	class float4_sse : public float_base<4, __m128, m128_accessor> {
	protected:
		static const uint32_t N = 4;
		using T = float;
		using Data_t = __m128;
		using this_t = float4_sse;
		using base_t = float_base<4, __m128, m128_accessor>;

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m128 allOnes() { return _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()); }

	public:
		//Forward constructors
		using base_t::base_t;

		float4_sse() {
			_mm_setzero_ps();
		};

		float4_sse(const base_t& o) : base_t(o) {};

		float4_sse(float a, float b, float c, float d){
			data = _mm_setr_ps(a, b, c, d);
		}

		float4_sse(float a) {
			data = _mm_set_ps1(a);
		}

		float4_sse(const Data_t& d) {
			data = d;
		}

		//Destructive binary ops
		//+= -= *= /= %= |= &= ^= >>= <<=
		this_t& operator+=(const this_t& o) { data = _mm_add_ps(data, o.data); return *this; }
		this_t operator+(const this_t& o) const { return _mm_add_ps(data, o.data); }

		this_t& operator-=(const this_t& o) { data = _mm_sub_ps(data, o.data); return *this; }
		this_t operator-(const this_t& o) const { return _mm_sub_ps(data, o.data); }

		this_t& operator*=(const this_t& o) { data = _mm_mul_ps(data, o.data); return *this; }
		this_t operator*(const this_t& o) const { return _mm_mul_ps(data, o.data); }

		this_t& operator/=(const this_t& o) { data = _mm_div_ps(data, o.data); return *this; }
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

		//[]
	};

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