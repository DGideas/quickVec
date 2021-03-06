#pragma once
#include "int_base.hpp"

namespace QuickVec {

	struct m128_int32 {
		__m128i raw;
		m128_int32() = default;
		m128_int32(const __m128i& o) : raw(o) {};
		operator const __m128i () const { return raw; }
		operator __m128i& () { return raw; }
		const int& operator[](size_t i) const { return raw.m128i_i32[i]; };
		int& operator[](size_t i) { return raw.m128i_i32[i]; };
	};

	class alignas(__m128i) int32x4_sse2 : public int32_base<4, m128_int32> {
	protected:
		static const uint32_t N = 4;
		using T = int32_t;
		using Data_t = m128_int32;
		using this_t = int32x4_sse2;
		using base_t = int32_base<4, m128_int32>;

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m128i allOnes() { return _mm_castps_si128(_mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps())); }

	public:
		////////////////////////////////
		// Constructors
		////////////////////////////////
		int32x4_sse2() : base_t() {}

		int32x4_sse2(const base_t& o) : base_t(o) {};

		int32x4_sse2(int32_t a, int32_t b, int32_t c, int32_t d){
			data = _mm_setr_epi32(a, b, c, d);
		};

		int32x4_sse2(T a) {
			data = _mm_set1_epi32(a);
		}

		int32x4_sse2(Data_t&& d){
			data = d;
		}

		////////////////////////////////
		// Binary Operators
		////////////////////////////////

		// Addition
		this_t& operator+=(const this_t& o) { data = _mm_add_epi32(data, o.data); return *this; }
		this_t operator+(const this_t& o) const { return _mm_add_epi32(data, o.data); }

		//Subtraction
		this_t& operator-=(const this_t& o) { data = _mm_sub_epi32(data, o.data); return *this; }
		this_t operator-(const this_t& o) const { return _mm_sub_epi32(data, o.data); }

		//Multiplication
	private:
		static __m128i multiply(const __m128i& a, const __m128i& b) {
			__m128i tmp1 = _mm_mul_epu32(a, b); /* mul 2,0*/
			__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)); /* mul 3,1 */
			return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0))); /* shuffle results to [63..0] and pack */
		}
	public:
		this_t& operator*=(const this_t& o) { data = (*this * o).data; return *this; }
		this_t operator*(const this_t& o) const { return multiply(data, o.data); }

		//Division not possible
		//XXXXXXXXXXX

		//Modulo not possible
		//XXXXXXXXXXX
		
		//Bit or
		this_t& operator|=(const this_t& o) { data = _mm_or_si128(data, o.data); return *this; }
		this_t operator|(const this_t& o) const { return _mm_or_si128(data, o.data); }

		//Bit and
		this_t& operator&=(const this_t& o) { data = _mm_and_si128(data, o.data); return *this; }
		this_t operator&(const this_t& o) const { return _mm_and_si128(data, o.data); }

		//Bit xor
		this_t& operator^=(const this_t& o) { data = _mm_xor_si128(data, o.data); return *this; }
		this_t operator^(const this_t& o) const { return _mm_xor_si128(data, o.data); }

		//Shift right
		// no vector to vector operator
		this_t& operator>>=(const this_t& o) { base_t::operator>>=(o); return *this; }
		this_t operator>>(const this_t& o) const { return base_t::operator>>(o); }
		//explicit singular operator for efficiency
		this_t& operator>>=(const T& o) { data = _mm_srai_epi32(data, o); return *this; }
		this_t operator>>(const T& o) const { return _mm_srai_epi32(data, o); }

		//Shift left
		// no vector to vector operator
		this_t& operator<<=(const this_t& o) { base_t::operator<<=(o); return *this; }
		this_t operator<<(const this_t& o) const { return base_t::operator<<(o); }
		//explicit singular operator for efficiency
		this_t& operator<<=(const T& o) { data = _mm_slli_epi32(data, o); return *this; }
		this_t operator<<(const T& o) const { return _mm_slli_epi32(data, o); }

		////////////////////////////////
		// Unary Operators
		////////////////////////////////
		//-~!
		this_t operator-() const { return this_t(_mm_setzero_si128()) - *this; }
		this_t operator~() const { return this_t(allOnes()) ^ *this; }
		this_t operator!() const { return _mm_cmpeq_epi32(_mm_setzero_si128(), data); }

		//Comparisons
		//== != < <= > >=

		//[]
	};

	class int32x4_sse4_1: public int32x4_sse2 {
	private:
		static const uint32_t N = 4;
		using T = int32_t;
		using Data_t = __m128i;
		using this_t = int32x4_sse4_1;
		using base_t = int32x4_sse2;
	public:
		////////////////////////////////
		// Constructors
		////////////////////////////////
		//No specializing constructors forward to base class
		using base_t::base_t;

		////////////////////////////////
		// Binary Operators
		////////////////////////////////
		this_t& operator*=(const this_t& o) { data = _mm_mullo_epi32(data, o.data); return *this; }
		this_t operator*(const this_t& o) const { return _mm_mullo_epi32(data, o.data); }
	};

}
