#pragma once

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

	class float4_sse : public float_base<4, float, __m128, m128_accessor> {
	protected:
		static const uint32_t N = 4;
		using T = float;
		using Data_t = __m128;
		using fp = float4_sse;
		using fp_base = float_base<4, float, __m128, m128_accessor>;

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m128 allOnes() { __m128 junk; return _mm_cmpneq_ps(junk, junk); }

	public:
		float4_sse() : fp_base() {}

		float4_sse(const fp_base& o) : fp_base(o) {};

		//template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N>::type>
		//float_sse(const Args&... args) : fp_base(args...) {};
		float4_sse(float a, float b, float c, float d) : fp_base(a, b, c, d) {};

		float4_sse(Data_t&& d) : fp_base(std::move(d)) {};

		//Destructive binary ops
		//+= -= *= /= %= |= &= ^= >>= <<=
		fp& operator+=(const fp& o) { data = _mm_add_ps(data, o.data); return *this; }
		fp& operator-=(const fp& o) { data = _mm_sub_ps(data, o.data); return *this; }
		fp& operator*=(const fp& o) { data = _mm_mul_ps(data, o.data); return *this; }
		fp& operator/=(const fp& o) { data = _mm_div_ps(data, o.data); return *this; }
		//fp& operator%=(const fp& o) { }
		fp& operator|=(const fp& o) { data = _mm_or_ps(data, o.data); return *this; }
		fp& operator&=(const fp& o) { data = _mm_and_ps(data, o.data); return *this; }
		fp& operator^=(const fp& o) { data = _mm_xor_ps(data, o.data); return *this; }
		//fp& operator>>=(const fp&) {}
		//fp& operator<<=(const fp&) {}

		//Destructive singular binary ops
		//+= -= *= /= %= |= &= ^= >>= <<=
		fp& operator+=(const T& o) { data = _mm_add_ps(data, _mm_set_ps1(o)); return *this; }
		fp& operator-=(const T& o) { data = _mm_sub_ps(data, _mm_set_ps1(o)); return *this; }
		fp& operator*=(const T& o) { data = _mm_mul_ps(data, _mm_set_ps1(o)); return *this; }
		fp& operator/=(const T& o) { data = _mm_div_ps(data, _mm_set_ps1(o)); return *this; }
		//fp& operator%=(const T& o) { }
		fp& operator|=(const T& o) { data = _mm_or_ps(data, _mm_set_ps1(o)); return *this; }
		fp& operator&=(const T& o) { data = _mm_and_ps(data, _mm_set_ps1(o)); return *this; }
		fp& operator^=(const T& o) { data = _mm_xor_ps(data, _mm_set_ps1(o)); return *this; }
		//fp& operator>>=(const fp&) {}
		//fp& operator<<=(const fp&) {}

		//Non-Destructive binary ops
		//+ - * / % | & >> <<
		fp operator+(const fp& o) const { return _mm_add_ps(data, o.data); }
		fp operator-(const fp& o) const { return _mm_sub_ps(data, o.data); }
		fp operator*(const fp& o) const { return _mm_mul_ps(data, o.data); }
		fp operator/(const fp& o) const { return _mm_div_ps(data, o.data); }
		//fp operator%(const fp& o) const {}
		fp operator|(const fp& o) const { return _mm_or_ps(data, o.data); }
		fp operator&(const fp& o) const { return _mm_and_ps(data, o.data); }
		fp operator^(const fp& o) const { return _mm_xor_ps(data, o.data); }

		//Non-Destructive singular binary ops
		//+ - * / % | & >> <<
		fp operator+(const T& o) const { return _mm_add_ps(data, _mm_set_ps1(o)); }
		fp operator-(const T& o) const { return _mm_sub_ps(data, _mm_set_ps1(o)); }
		fp operator*(const T& o) const { return _mm_mul_ps(data, _mm_set_ps1(o)); }
		fp operator/(const T& o) const { return _mm_div_ps(data, _mm_set_ps1(o)); }
		//fp operator%(const T& o) const { }
		fp operator|(const T& o) const { return _mm_or_ps(data, _mm_set_ps1(o)); }
		fp operator&(const T& o) const { return _mm_and_ps(data, _mm_set_ps1(o)); }
		fp operator^(const T& o) const { return _mm_xor_ps(data, _mm_set_ps1(o)); }

		//Unary ops
		//-~!
		fp operator-() const { return fp(_mm_setzero_ps()) - *this; }
		fp operator~() const { return _mm_xor_ps(allOnes(), data); }
		fp operator!() const { return _mm_cmpneq_ps(_mm_setzero_ps(), data); }

		//Comparisons
		//== != < <= > >=

		//[]
	};

	class float4_sse2 : public float4_sse {
	protected:
		using fp = float4_sse2;
	public:
		float4_sse2() : float4_sse() {};
		float4_sse2(float a, float b, float c, float d) : float4_sse(a, b, c, d) {};
		float4_sse2(Data_t&& d) : float4_sse(std::move(d)) {};

		fp& operator%=(const fp& o) {
			// a - b * floor(a/b)
			data = (*this % o).data;
			return *this;
		}
		fp operator%(const fp& o) const {
			fp ret(_mm_sub_ps(data, _mm_mul_ps(o.data, _mm_cvtepi32_ps(_mm_cvtps_epi32(_mm_div_ps(data, o.data))))));
			__m128 mask = _mm_cmplt_ps(ret.data, _mm_setzero_ps());
			ret.data = _mm_add_ps(ret.data, _mm_and_ps(mask, o.data));
			return ret;
		}
	};

	class float4_sse4_1 : public float4_sse2 {
		using fp = float4_sse4_1;
	public:
		float4_sse4_1() : float4_sse2() {};
		float4_sse4_1(float a, float b, float c, float d) : float4_sse2(a, b, c, d) {};
		float4_sse4_1(Data_t&& d) : float4_sse2(std::move(d)) {};

		fp& operator%=(const fp& o) {
			// a - b * floor(a/b)
			data = (*this % o).data;
			return *this;
		}
		fp operator%(const fp& o) const {
			return fp(_mm_sub_ps(data, _mm_mul_ps(o.data, _mm_floor_ps(_mm_div_ps(data, o.data)))));
		}
	};


}