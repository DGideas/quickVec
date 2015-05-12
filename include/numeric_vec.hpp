// numeric_vec.hpp
// Copyright 2015 Matthew Kellogg
//
// This defines the base template for numeric vector types
// Implementations must implement the following
// Constructor
// vec_t();
// vec_t(T o);
//
// Loads and stores
// static vec_t loadAligned(const float* ptr);
// static vec_t load(const float* ptr);
// void store(float* ptr);
// void storeAligned(float* ptr);
//
// Masking operations
// vec_t& if_set(const bool_t& mask, const vec_t& newVal);
// vec_t& if_not_set(const bool_t& mask, const vec_t& newVal);
//
// Element accessors
// T& operator[](size_t i);
// const T& operator[](size_t i) const;
//
// Binary operators
// vec_t operator+(const vec_t& a, const vec_t& b);
// vec_t operator-(const vec_t& a, const vec_t& b);
// vec_t operator*(const vec_t& a, const vec_t& b);
// vec_t operator/(const vec_t& a, const vec_t& b);
// vec_t operator%(const vec_t& a, const vec_t& b);
// 
// Unary operators
// vec_t operator-(const vec_t& a);
//
// Additionally if integral
// vec_t operator<<(const vec_t& a, const vec_t& b);
// vec_t operator>>(const vec_t& a, const vec_t& b);
// vec_t operator|(const vec_t& a, const vec_t& b);
// vec_t operator&(const vec_t& a, const vec_t& b);
// vec_t operator^(const vec_t& a, const vec_t& b);
// vec_t operator~(const vec_t& a);

#pragma once

#include <array>
#include "bool_vec.hpp"

namespace QuickVec {
	constexpr size_t nextPow2_(size_t value, size_t maxb = sizeof(size_t)*CHAR_BIT, size_t curb = 1) {
		return maxb <= curb
			? value
			: nextPow2_(((value - 1) | ((value - 1) >> curb)) + 1, maxb, curb << 1);
	}

	template<typename T,
		size_t size_,
		typename data_t = std::array<T, size_>,
		typename bool_t_ = typename bool_vec<size_> >
	class alignas(data_t) numeric_vec
	{
		using vec_t = numeric_vec<T, size_, data_t, bool_t_>;
	public:
		using bool_t = bool_t_;
		static const size_t size = size_;
		data_t raw;
		vec_t();
		vec_t(vec_t&& o) = default;
		vec_t(const vec_t& o) = default;
		vec_t& operator=(const vec_t& o) = default;
		explicit vec_t(const data_t& o) : raw(o) {};

		template<typename... Args, typename std::enable_if<N-1 == sizeof...(Args)>::type >
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

		static vec_t loadAligned(const float* ptr);
		static vec_t load(const float* ptr);

		void store(float* ptr);
		void storeAligned(float* ptr);

		vec_t& if_set(const bool_t& mask, const vec_t& newVal);
		vec_t& if_not_set(const bool_t& mask, const vec_t& newVal);

		T& operator[](size_t i);
		const T& operator[](size_t i) const;

		vec_t& operator+=(const vec_t& o) { raw = *this + o; return *this; }
		vec_t& operator-=(const vec_t& o) { raw = *this - o; return *this; }
		vec_t& operator*=(const vec_t& o) { raw = *this * o; return *this; }
		vec_t& operator/=(const vec_t& o) { raw = *this / o; return *this; }
	};

	template<size_t size, typename data_t = std::array<float, size>, typename bool_t = bool_vec<size> >
	using float_vec = numeric_vec<float, size, data_t, bool_t>;

	template<size_t size, typename data_t = std::array<float, size>, typename bool_t = bool_vec<size> >
	using int32_vec = numeric_vec<int, size, data_t, bool_t>;

	template<typename T, size_t size, typename data_t, typename bool_t>
	numeric_vec<T, size, data_t, bool_t>::numeric_vec() {

	}

	
}