#pragma once

#error "DONT USE THIS"

#include <xmmintrin.h>
#include "float_base.hpp"

//TODO: a splattering of noexcept

namespace QuickVec {
	template<typename data_t, size_t size>
	class alignas(data_t)bool_vec
	{
		using bool_t = bool_vec<data_t, size>;
		// Only implicitly convert to and from raw in private
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
		bool any() const;
		bool operator[](size_t i) const;
		class reference {
			friend class bool_vec;
			reference(bool_vec& bv, size_t index) : vec(bv), index(index) {};
			bool_vec& vec;
			size_t index;
		public:
			~reference() = default;
			operator bool() const;
			reference& operator=(bool x); //assign from bool
		};
		reference operator[](size_t i);
	};

	template<typename data_t, size_t size>
	inline bool all(const bool_vec<data_t, size>& b) {
		return b.all();
	}

	template<typename data_t, size_t size>
	inline bool any(const bool_vec<data_t, size>& b) {
		return b.all();
	}

	//implementations must implement the following
	// Constructor
	// bool_t(const bool& o);
	//
	// Check if all/any are true
	// bool all() const;
	// bool any() const;
	//
	// Element accessors
	// bool operator[](size_t i) const;
	// reference operator[](size_t i);
	// reference::operator bool() const;
	// reference& reference::operator=(bool x);
	//
	// Binary operators
	// bool_t operator||(const bool_t& a, const bool_t& b);
	// bool_t operator&&(const bool_t& b, const bool_t& b);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// bool avx file
	using bool8_avx = bool_vec<__m256, 8>;

	/////////////////////////////////////////////////////////////
	// bool8_avx singular constructor
	/////////////////////////////////////////////////////////////
	inline bool8_avx::bool_vec(const bool& o) {
		if (o) {
			raw = _mm256_cmp_ps(raw, raw, _CMP_EQ_UQ);
		}
		else {
			raw = _mm256_xor_ps(raw, raw);
		}
	}

	/////////////////////////////////////////////////////////////
	// bool8_avx any/all operators
	/////////////////////////////////////////////////////////////
	inline bool bool8_avx::all() const {
		return (_mm256_testc_ps(bool_t(true), raw) == 0);
	}

	inline bool bool8_avx::any() const {
		return (_mm256_testz_ps(raw, raw) == 0);
	}

	/////////////////////////////////////////////////////////////
	// bool8_avx accessors
	/////////////////////////////////////////////////////////////
	inline bool bool8_avx::operator[](size_t i) const {
		return reinterpret_cast<const int32_t&>(raw.m256_f32[i]) != 0;
	}
	inline bool8_avx::reference bool8_avx::operator[](size_t i){
		return reference(*this, i);
	}
	inline bool8_avx::reference::operator bool() const {
		return vec[index];
	}
	inline bool8_avx::reference& bool8_avx::reference::operator=(bool x) {
		int val = (x) ? -1 : 0;
		vec.raw.m256_f32[index] = reinterpret_cast<const float&>(val);
		return *this;
	}

	/////////////////////////////////////////////////////////////
	// bool8_avx binary operators
	/////////////////////////////////////////////////////////////
	inline bool8_avx operator||(const bool8_avx& a, const bool8_avx& b) {
		return bool8_avx(_mm256_or_ps(a.raw, b.raw));
	}
	inline bool8_avx operator&&(const bool8_avx& a, const bool8_avx& b) {
		return bool8_avx(_mm256_and_ps(a.raw, b.raw));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// float vec file
	template<typename T, typename data_t, typename bool_t_, size_t size_>
	class alignas(data_t) numeric_vec
	{
		using vec_t = numeric_vec<T, data_t, bool_t_, size_>;
	public:
		using bool_t = bool_t_;
		static const size_t size = size_;
		data_t raw;
		vec_t();
		vec_t(vec_t&& o) = default;
		vec_t(const vec_t& o) = default;
		vec_t& operator=(const vec_t& o) = default;
		vec_t& operator=(vec_t&& o) = default;
		explicit vec_t(const data_t& o) : raw(o) {};

		template<typename... Args, typename Enable = std::enable_if<size - 1 == sizeof...(Args)>::type >
		vec_t(const T& a, const Args&... args) : raw({ a, static_cast<T>(args)... }) {};

		vec_t(T o);
		vec_t(const T* ptr, bool aligned = false) {
			if (aligned) {
				raw = loadAligned(ptr).raw;
			}
			else {
				raw = load(ptr).raw;
			}
		}

		static vec_t zero();

		static vec_t loadAligned(const float* ptr);
		static vec_t load(const float* ptr);

		void store(float* ptr);
		void storeAligned(float* ptr);

		vec_t& if_set(const bool_t& mask, const vec_t& newVal);
		vec_t& if_not_set(const bool_t& mask, const vec_t& newVal);

		T& operator[](size_t i);
		const T& operator[](size_t i) const;

		vec_t& operator+=(const vec_t& o) { raw = (*this + o).raw; return *this; }
		vec_t& operator-=(const vec_t& o) { raw = (*this - o).raw; return *this; }
		vec_t& operator*=(const vec_t& o) { raw = (*this * o).raw; return *this; }
		vec_t& operator/=(const vec_t& o) { raw = (*this / o).raw; return *this; }
	};

	template<typename data_t, typename bool_t, size_t size>
	using float_vec = numeric_vec<float, data_t, bool_t, size>;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// float8_avx file
	using float8_avx = float_vec<__m256, bool8_avx, 8>;

	inline float8_avx::numeric_vec() {
		raw = _mm256_setzero_ps();
	}

	inline float8_avx::numeric_vec(float o) {
		raw = _mm256_set1_ps(o);
	}

	inline float8_avx float8_avx::zero() {
		return float8_avx(_mm256_setzero_ps());
	}

	inline float8_avx float8_avx::loadAligned(const float* ptr) {
		return float8_avx(_mm256_load_ps(ptr));
	}

	inline float8_avx float8_avx::load(const float* ptr) {
		return float8_avx(_mm256_loadu_ps(ptr));
	}

	inline void float8_avx::store(float* ptr) {
		_mm256_storeu_ps(ptr, raw);
	}
		
	inline void float8_avx::storeAligned(float* ptr) {
		_mm256_storeu_ps(ptr, raw);
	}

	inline float8_avx& float8_avx::if_set(const float8_avx::bool_t& mask, const float8_avx& newVal) {
		raw = _mm256_blendv_ps(raw, newVal.raw, mask.raw);
		return *this;
	}

	inline float8_avx& float8_avx::if_not_set(const float8_avx::bool_t& mask, const float8_avx& newVal) {
		raw = _mm256_blendv_ps(newVal.raw, raw, mask.raw);
		return *this;
	}

	inline const float& float8_avx::operator[](size_t i) const {
		return raw.m256_f32[i];
	}

	inline float& float8_avx::operator[](size_t i) {
		return raw.m256_f32[i];
	}

	/////////////////////////////////////////////////////////////
	// float8 arithmetic operators
	/////////////////////////////////////////////////////////////
	inline float8_avx operator+(const float8_avx& a, const float8_avx& b) {
		return float8_avx(_mm256_add_ps(a.raw, b.raw));
	}

	inline float8_avx operator-(const float8_avx& a, const float8_avx& b) {
		return float8_avx(_mm256_sub_ps(a.raw, b.raw));
	}

	inline float8_avx operator*(const float8_avx& a, const float8_avx& b) {
		return float8_avx(_mm256_mul_ps(a.raw, b.raw));
	}

	inline float8_avx operator/(const float8_avx& a, const float8_avx& b) {
		return float8_avx(_mm256_div_ps(a.raw, b.raw));
	}

	inline float8_avx operator-(const float8_avx& a) {
		return float8_avx(_mm256_sub_ps(_mm256_setzero_ps(), a.raw));
	}

	/////////////////////////////////////////////////////////////
	// float8 comparison operators
	/////////////////////////////////////////////////////////////
	inline float8_avx::bool_t operator<(const float8_avx& a, const float8_avx& b) {
		return float8_avx::bool_t(_mm256_cmp_ps(a.raw, b.raw, _CMP_LT_OS));
	}

	inline float8_avx::bool_t operator<=(const float8_avx& a, const float8_avx& b) {
		return float8_avx::bool_t(_mm256_cmp_ps(a.raw, b.raw, _CMP_LE_OS));
	}

	inline float8_avx::bool_t operator==(const float8_avx& a, const float8_avx& b) {
		return float8_avx::bool_t(_mm256_cmp_ps(a.raw, b.raw, _CMP_EQ_OS));
	}

	inline float8_avx::bool_t operator!=(const float8_avx& a, const float8_avx& b) {
		return float8_avx::bool_t(_mm256_cmp_ps(a.raw, b.raw, _CMP_NEQ_OS));
	}

	inline float8_avx::bool_t operator>=(const float8_avx& a, const float8_avx& b) {
		return float8_avx::bool_t(_mm256_cmp_ps(a.raw, b.raw, _CMP_GE_OS));
	}

	inline float8_avx::bool_t operator>(const float8_avx& a, const float8_avx& b) {
		return float8_avx::bool_t(_mm256_cmp_ps(a.raw, b.raw, _CMP_GT_OS));
	}
}