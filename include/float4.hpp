#pragma once

#include <xmmintrin.h>
#include "float_base.hpp"
namespace QuickVec {
	
	class bool4_sse {
		using this_t = bool4_sse;
		using T = int;
		using Data_t = __m128;
		static const size_t N = 4;
		Data_t data;
	public:
		this_t() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		this_t(T&& t, Args&&... args) :data({ t, static_cast<T>(args)... }) {};

		template <typename Enable = std::enable_if<(N>1)>::type>
		this_t(T&& t) {
			data = _mm_castepi32_ps(_mm_set1_epi32(t));
		}

		this_t(Data_t&& d) : data(d) {};

		bool all() const {
			return (_mm_testc_ps(_mm_cmpeq_ps(data,data), data)!=0);
			/*for (int i = 0; i < N; i++) {
			if (!data[i]) return false;
			}
			return true;*/
		}

		//[]
		T& operator[](size_t i) { return reinterpret_cast<int&>(data.m128_f32[i]); }
		T operator[](size_t i) const { return  reinterpret_cast<const int&>(data.m128_f32[i]); }
	};

	class alignas(__m128) float4_sse : public float_base<4> {
	protected:
		static const uint32_t N = 4;
		using T = float;
		using Data_t = __m128;
		using this_t = float4_sse;
		using base_t = float_base<4>;

		__m128i& m128i_data() {
			return reinterpret_cast<__m128i&>(data);
		}

		const __m128i& m128i_data() const {
			return reinterpret_cast<const __m128i&>(data);
		}

		__m128& m128_data() {
			return reinterpret_cast<__m128&>(data);
		}

		const __m128& m128_data() const {
			return reinterpret_cast<const __m128&>(data);
		}

		//helpers
		// Sets all bits in the vector to one giving 0xFF.....FFFF
		static __m128 allOnes() { return _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()); }

	public:
		using bool_t = bool4_sse;

		float4_sse() {
			_mm_setzero_ps();
		};

		float4_sse(const base_t& o) : base_t(o) {};

		float4_sse(float a, float b, float c, float d){
			m128_data() = _mm_setr_ps(a, b, c, d);
		}

		float4_sse(float a) {
			m128_data() = _mm_set_ps1(a);
		}

		float4_sse(const Data_t& d) {
			m128_data() = d;
		}

		//Binary ops
		//+ - * / % | & ^ >> <<
		this_t& operator+=(const this_t& o) { m128_data() = _mm_add_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator+(const this_t& o) const { return _mm_add_ps(m128_data(), o.m128_data()); }

		this_t& operator-=(const this_t& o) { m128_data() = _mm_sub_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator-(const this_t& o) const { return _mm_sub_ps(m128_data(), o.m128_data()); }

		this_t& operator*=(const this_t& o) { m128_data() = _mm_mul_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator*(const this_t& o) const { return _mm_mul_ps(m128_data(), o.m128_data()); }

		this_t& operator/=(const this_t& o) { m128_data() = _mm_div_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator/(const this_t& o) const { return _mm_div_ps(m128_data(), o.m128_data()); }

		//this_t& operator%=(const this_t& o) { }
		//this_t operator%(const this_t& o) const {}

		this_t& operator|=(const this_t& o) { m128_data() = _mm_or_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator|(const this_t& o) const { return _mm_or_ps(m128_data(), o.m128_data()); }

		this_t& operator&=(const this_t& o) { m128_data() = _mm_and_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator&(const this_t& o) const { return _mm_and_ps(m128_data(), o.m128_data()); }
		
		this_t& operator^=(const this_t& o) { m128_data() = _mm_xor_ps(m128_data(), o.m128_data()); return *this; }
		this_t operator^(const this_t& o) const { return _mm_xor_ps(m128_data(), o.m128_data()); }

		//Unary ops
		//-~!
		this_t operator-() const { return this_t(_mm_setzero_ps()) - *this; }
		this_t operator~() const { return _mm_xor_ps(allOnes(), m128_data()); }
		this_t operator!() const { return _mm_cmpeq_ps(_mm_setzero_ps(), m128_data()); }

		//Comparisons
		//== != < <= > >=
		bool_t operator<(const this_t& o) const { return bool_t(_mm_cmplt_ps(m128_data(), o.m128_data())); }
		bool_t operator<(const T& o) const { return bool_t(_mm_cmplt_ps(m128_data(), _mm_set_ps1(o))); }

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
			this_t ret(_mm_sub_ps(m128_data(), _mm_mul_ps(o.m128_data(), _mm_cvtepi32_ps(_mm_cvtps_epi32(_mm_div_ps(m128_data(), o.m128_data()))))));
			__m128 mask = _mm_cmplt_ps(ret.m128_data(), _mm_setzero_ps());
			ret.m128_data() = _mm_add_ps(ret.m128_data(), _mm_and_ps(mask, _mm_andnot_ps(_mm_set1_ps(-0.0f), o.m128_data())));
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
			m128_data() = (*this % o).m128_data();
			return *this;
		}

		this_t operator%(const this_t& o) const {
			this_t ret(_mm_sub_ps(m128_data(), _mm_mul_ps(o.m128_data(), _mm_floor_ps(_mm_div_ps(m128_data(), o.m128_data())))));
			__m128 mask = _mm_cmplt_ps(ret.m128_data(), _mm_setzero_ps());
			ret.m128_data() = _mm_add_ps(ret.m128_data(), _mm_and_ps(mask, _mm_andnot_ps(_mm_set1_ps(-0.0f), o.m128_data())));
			return ret;
		}
	};


}