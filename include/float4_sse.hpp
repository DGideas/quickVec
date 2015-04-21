#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"
namespace QuickVec {

	struct sse_accessor {
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

	class float_sse : public float_base<4, float, __m128, sse_accessor> {
		static const uint32_t N = 4;
		using T = float;
		using Data_t = __m128;
		using fp = float_sse;
		using fp_base = float_base<4, float, __m128, sse_accessor>;

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m128 allOnes() { __m128 junk; return _mm_cmpneq_ps(junk, junk); }

	public:
		float_sse() : fp_base() {}

		float_sse(const fp_base& o) : fp_base(o) {};

		//template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N>::type>
		//float_sse(const Args&... args) : fp_base(args...) {};
		float_sse(float a, float b, float c, float d) : fp_base(a, b, c, d) {};

		float_sse(Data_t&& d) : fp_base(std::move(d)) {};

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
}