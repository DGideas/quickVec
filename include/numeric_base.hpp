#pragma once

#pragma once

#include <array>
#include <iostream>
#include "array_accessor.hpp"

namespace QuickVec {

	template<typename T, size_t N>
	struct array_accessor {
		template<size_t I>
		static T& get(std::array<T, N>& data) {
			return data[I];
		}

		template<size_t I>
		static T get(const std::array<T, N>& data) {
			return data[I];
		}

		static T& get(std::array<T, N>& data, size_t I) {
			return data[I];
		}

		static T get(const std::array<T, N>& data, size_t I) {
			return data[I];
		}
	};

	template<size_t N, typename T, typename Data_t = typename std::array<T, N>, typename DataAccessor = array_accessor<T, N>>
	class numeric_base {
		using this_t = numeric_base<N, T, Data_t, DataAccessor>;
		using DA = DataAccessor;
		//Friend methods
		template<size_t N, typename T>
		friend std::ostream& operator<<(std::ostream& o, const this_t& f);

	public:
		Data_t data;
		static const size_t Size = N;

		numeric_base() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N-1>::type>
		numeric_base(T&& t, Args&&... args) :data({t, static_cast<T>(args)... }) {};

		template <typename Enable = std::enable_if<(N>1)>::type>
		numeric_base(T&& t) {
			for (size_t i = 0; i < N; i++) {
				DA::get(data, i) = t;
			}
		}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		numeric_base(const T& t, const Args&... args) :data({ t, static_cast<T>(args)... }) {};

		numeric_base(Data_t&& d) : data(std::forward<Data_t>(d)) {};

		//Destructive binary ops
		//+= -= *= /= %= |= &= ^= >>= <<=
		template<typename BinOp, size_t... I>
		this_t& doOpDestImpl(const this_t& o, BinOp op, std::index_sequence<I...>) {
			T _[] = {
				(DA::get<I>(data) = op(DA::get<I>(data), DA::get<I>(o.data)))... };
			return *this;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		this_t& doOpDest(const this_t& o, BinOp op) {
			return doOpDestImpl(o, op, Indices());
		}

		this_t& operator+=(const this_t& o) { return doOpDest(o, func::plus<T>()); }
		this_t& operator-=(const this_t& o) { return doOpDest(o, func::minus<T>()); }
		this_t& operator*=(const this_t& o) { return doOpDest(o, func::multiply<T>()); }
		this_t& operator/=(const this_t& o) { return doOpDest(o, func::divide<T>()); }
		this_t& operator%=(const this_t& o) { return doOpDest(o, func::modulo<T>()); }
		this_t& operator|=(const this_t& o) { return doOpDest(o, func::bit_or<T>()); }
		this_t& operator&=(const this_t& o) { return doOpDest(o, func::bit_and<T>()); }
		this_t& operator^=(const this_t& o) { return doOpDest(o, func::bit_xor<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t& operator>>=(const this_t& o) { return doOpDest(o, func::shift_right<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t& operator<<=(const this_t& o) { return doOpDest(o, func::shift_left<T>()); }

		//Destructive singular binary ops
		//+= -= *= /= %= |= &= ^= >>= <<= 
		template<typename BinOp, size_t... I>
		this_t& doOpDestImpl(const T& o, BinOp op, std::index_sequence<I...>) {
			T _[] = { (DA::get<I>(data) = op(DA::get<I>(data), o))... };
			return *this;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		this_t& doOpDest(const T& o, BinOp op) {
			return doOpDestImpl(o, op, Indices());
		}

		this_t& operator+=(const T& o) { return doOpDest(o, func::plus<T>()); }
		this_t& operator-=(const T& o) { return doOpDest(o, func::minus<T>()); }
		this_t& operator*=(const T& o) { return doOpDest(o, func::multiply<T>()); }
		this_t& operator/=(const T& o) { return doOpDest(o, func::divide<T>()); }
		this_t& operator%=(const T& o) { return doOpDest(o, func::modulo<T>()); }
		this_t& operator|=(const T& o) { return doOpDest(o, func::bit_or<T>()); }
		this_t& operator&=(const T& o) { return doOpDest(o, func::bit_and<T>()); }
		this_t& operator^=(const T& o) { return doOpDest(o, func::bit_xor<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t& operator>>=(const T& o) { return doOpDest(o, func::shift_right<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t& operator<<=(const T& o) { return doOpDest(o, func::shift_left<T>()); }

		//Non-Destructive binary ops
		//+ - * / % | & >> <<
		template<typename BinOp, size_t... I>
		this_t doOpImpl(const this_t& o, BinOp op, std::index_sequence<I...>) const {
			this_t ret;
			T _[] = { (DA::get(ret.data, I) =
				op(
					DA::get(data,I),
					DA::get(o.data,I)))... };
			return ret;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		this_t doOp(const this_t& o, BinOp op) const {
			return doOpImpl(o, op, Indices());
		}
		this_t operator+(const this_t& o) const { return doOp(o, func::plus<T>()); }
		this_t operator-(const this_t& o) const { return doOp(o, func::minus<T>()); }
		this_t operator*(const this_t& o) const { return doOp(o, func::multiply<T>()); }
		this_t operator/(const this_t& o) const { return doOp(o, func::divide<T>()); }
		this_t operator%(const this_t& o) const { return doOp(o, func::modulo<T>()); }
		this_t operator|(const this_t& o) const { return doOp(o, func::bit_or<T>()); }
		this_t operator&(const this_t& o) const { return doOp(o, func::bit_and<T>()); }
		this_t operator^(const this_t& o) const { return doOp(o, func::bit_xor<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t operator>>(const this_t& o) const { return doOp(o, func::shift_right<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t operator<<(const this_t& o) const { return doOp(o, func::shift_left<T>()); }

		//Non-Destructive singular binary ops
		//+ - * / % | & >> <<
		template<typename BinOp, size_t... I>
		this_t doOpImpl(const T& o, BinOp op, std::index_sequence<I...>) const {
			this_t ret;
			T _[] = { (DA::get<I>(ret.data) = op(DA::get<I>(data), o))... };
			return ret;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		this_t doOp(const T& o, BinOp op) const {
			return doOpImpl(o, op, Indices());
		}
		this_t operator+(const T& o) const { return doOp(o, func::plus<T>()); }
		this_t operator-(const T& o) const { return doOp(o, func::minus<T>()); }
		this_t operator*(const T& o) const { return doOp(o, func::multiply<T>()); }
		this_t operator/(const T& o) const { return doOp(o, func::divide<T>()); }
		this_t operator%(const T& o) const { return doOp(o, func::modulo<T>()); }
		this_t operator|(const T& o) const { return doOp(o, func::bit_or<T>()); }
		this_t operator&(const T& o) const { return doOp(o, func::bit_and<T>()); }
		this_t operator^(const T& o) const { return doOp(o, func::bit_xor<T>()); }
		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t operator>>(const T& o) const { return doOp(o, func::shift_right<T>()); }

		//Enable only for integral types
		template<typename Enable = std::enable_if<std::is_integral<T>::value>::type>
		this_t operator<<(const T& o) const { return doOp(o, func::shift_left<T>()); }

		//Unary ops
		//-~!
		template<typename UnaryOp>
		this_t doOp(UnaryOp op) {
			this_t res;
			for (size_t i = 0; i < N; i++) res.data[i] = op(data[i]);
			return res;
		}
		this_t operator-() { return doOp(func::negate<T>()); }
		this_t operator~() { return doOp(func::bit_not<T>()); }
		this_t operator!() { return doOp(func::logical_not<T>()); }

		//Comparisons
		//== != < <= > >=

		//[]
		T& operator[](size_t i) { return DA::get(data, i); }
		T operator[](size_t i) const { return DA::get(data, i); }
	};

	template<size_t N, typename T, typename Data_t, typename DA>
	std::ostream& operator<<(std::ostream& o, const numeric_base<N, T, Data_t, DA>& vec) {
		o << "{";
		for (uint32_t i = 0; i < N-1; i++) {
			o << vec[i] << ",";
		}
		o << vec[N - 1] << "}";
		return o;
	}

}
