#pragma once

#pragma once

#include <array>
#include <iostream>

namespace QuickVec {

	
	constexpr size_t nextPow2(size_t value, size_t maxb = sizeof(size_t)*CHAR_BIT, size_t curb = 1) {
		return maxb <= curb
			? value
			: nextPow2(((value - 1) | ((value - 1) >> curb)) + 1, maxb, curb << 1);
	}

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

	// Forward declaration
	template<typename T, size_t N>
	class numeric_base;

	template<size_t N>
	class bool_base {
		using this_t = bool_base<N>;
		using T = int;
		using Data_t = std::array<T, N>;
		Data_t data;
	public:
		this_t() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		this_t(T&& t, Args&&... args) :data({ t, static_cast<T>(args)... }) {};

		template <typename Enable = std::enable_if<(N>1)>::type>
		this_t(T&& t) {
			for (size_t i = 0; i < N; i++) {
				data[i] = t;
			}
		}

		this_t(Data_t&& d) : data(std::forward<Data_t>(d)) {};

		bool all() const {
			for (int i = 0; i < N; i++) {
				if (!data[i]) return false;
			}
			return true;
		}

		//[]
		T& operator[](size_t i) { return data[i]; }
		T operator[](size_t i) const { return data[i]; }
	};

	template<typename T, size_t N>
	class alignas(nextPow2(N)) numeric_base {
		using this_t = numeric_base<T, N>;
		using data_t = std::array<T, N>;
		//Friend methods
		//friend std::ostream& operator<<(std::ostream& o, const this_t& f);
	public:
		data_t data;
		static const size_t Size = N;
		using bool_t = bool_base<4>;

		this_t() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N-1>::type>
		this_t(T&& t, Args&&... args) :data({t, static_cast<T>(args)... }) {};

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		this_t(const T& t, const Args&... args) :data({ t, static_cast<T>(args)... }) {};

		template <typename Enable = std::enable_if<(N>1)>::type>
		this_t(T&& t) {
			for (size_t i = 0; i < N; i++) {
				data[i] = t;
			}
		}

		this_t(data_t&& d) : data(std::forward<data_t>(d)) {};

		//Destructive binary ops
		//+= -= *= /= %= |= &= ^= >>= <<=
		template<typename BinOp, size_t... I>
		this_t& doOpDestImpl(const this_t& o, BinOp op, std::index_sequence<I...>) {
			T _[] = { (data[I] = op(data[I], o.data[I]))... };
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
			T _[] = { (data[I] = op(data[I], o))... };
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
			T _[] = { (ret.data[I] = op( data[I], o.data[I]))... };
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
			T _[] = { (ret.data[I] = op(data[I], o))... };
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
		template<typename UnaryOp, size_t... I>
		this_t doOpImpl(UnaryOp op, std::index_sequence<I...>) const {
			this_t ret;
			T _[] = { (ret.data[I] = op(data[I]))... };
			return ret;
		}

		template<typename UnaryOp>
		this_t doOp(UnaryOp op) const {
			return doOpImpl(op, std::make_index_sequence<N>());
		}
		this_t operator-() const { return doOp(func::negate<T>()); }
		this_t operator~() const { return doOp(func::bit_not<T>()); }
		this_t operator!() const { return doOp(func::logical_not<T>()); }

		//Comparisons
		//== != < <= > >=
		template<typename BinOp, size_t... I>
		bool_t doCompImpl(const this_t& o, BinOp op, std::index_sequence<I...>) const {
			bool_t ret;
			bool _[] = { (ret[I] = op(data[I], o.data[I]))... };
			return ret;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		bool_t doComp(const this_t& o, BinOp op) const {
			return doCompImpl(o, op, Indices());
		}

		template<typename BinOp, size_t... I>
		bool_t doCompImpl(const T& o, BinOp op, std::index_sequence<I...>) const {
			bool_t ret;
			bool _[] = { (ret[I] = op(data[I],o))... };
			return ret;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		bool_t doComp(const T& o, BinOp op) const {
			return doCompImpl(o, op, Indices());
		}

		bool_t operator<(const this_t& o) const { return doComp(o, [](const T& a, const T& b) { return a < b;}); }
		bool_t operator<(const T& o) const { return doComp(o, [](const T& a, const T& b) { return a < b;}); }

		//[]
		T& operator[](size_t i) { return data[i]; }
		T operator[](size_t i) const { return data[i]; }
	};

	template<typename T, size_t N>
	std::ostream& operator<<(std::ostream& o, const numeric_base<T, N>& vec) {
		o << "{";
		for (uint32_t i = 0; i < N-1; i++) {
			o << vec[i] << ",";
		}
		o << vec[N - 1] << "}";
		return o;
	}
}
