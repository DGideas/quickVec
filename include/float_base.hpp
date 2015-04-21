#pragma once

#include <array>
#include <iostream>

namespace QuickVec {

	template<typename Int_t>
	constexpr Int_t pushRight(Int_t i) {
		return (i == 0) ? i : i | (pushRight(i >> 1));
	}

	template<typename Int_t>
	constexpr Int_t nextPow2(Int_t i) {
		return pushRight(i) + 1;
	}

	template<typename T, size_t N>
	struct array_accessor {
		template<size_t I>
		static float& get(std::array<T, N>& data) {
			return data[I];
		}

		template<size_t I>
		static float get(const std::array<T, N>& data) {
			return data[I];
		}

		static float& get(std::array<T, N>& data, size_t I) {
			return data[I];
		}

		static float get(const std::array<T, N>& data, size_t I) {
			return data[I];
		}
	};

	template<size_t N, typename T, typename Data_t = typename std::array<T, N>, typename DataAccessor = array_accessor<T, N>>
	class float_base {
		using fp = float_base<N, T, Data_t, DataAccessor>;
		using DA = DataAccessor;
		//Friend methods
		template<size_t N, typename T>
		friend std::ostream& operator<<(std::ostream& o, const fp& f);

	public:
		Data_t data;
		static const size_t Size = N;

		float_base() {}

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N>::type>
		float_base(Args&&... args) :data({ static_cast<T>(args)... }) {};

		float_base(Data_t&& d) : data(d) {};

		//Destructive binary ops
		//+= -= *= /= %= |= &= ^= >>= <<=
		template<typename BinOp, size_t... I>
		fp& doOpDestImpl(const fp& o, BinOp op, std::index_sequence<I...>) {
			float _[] = {
				(DA::get<I>(data) = op(DA::get<I>(data), DA::get<I>(o.data)))... };
			return *this;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		fp& doOpDest(const fp& o, BinOp op) {
			return doOpDestImpl(o, op, Indices());
		}

		fp& operator+=(const fp& o) { return doOpDest(o, std::plus<T>()); }
		fp& operator-=(const fp& o) { return doOpDest(o, std::minus<T>()); }
		fp& operator*=(const fp& o) { return doOpDest(o, std::multiplies<T>()); }
		fp& operator/=(const fp& o) { return doOpDest(o, std::divides<T>()); }
		fp& operator%=(const fp& o) { return doOpDest(o, func::modulus<T>()); }
		fp& operator|=(const fp& o) { return doOpDest(o, func::bit_or<T>()); }
		fp& operator&=(const fp& o) { return doOpDest(o, func::bit_and<T>()); }
		fp& operator^=(const fp& o) { return doOpDest(o, func::bit_xor<T>()); }
		fp& operator>>=(const fp& o) { return doOpDest(o, [](T a, T b) {return a >> b; }); }
		fp& operator<<=(const fp& o) { return doOpDest(o, [](T a, T b) {return a << b; }); }

		//Destructive singular binary ops
		//+= -= *= /= %= |= &= ^= >>= <<= 
		template<typename BinOp, size_t... I>
		fp& doOpDestImpl(const T& o, BinOp op, std::index_sequence<I...>) {
			float _[] = { (DA::get<I>(data) = op(DA::get<I>(data), o))... };
			return *this;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		fp& doOpDest(const T& o, BinOp op) {
			return doOpDestImpl(o, op, Indices());
		}

		fp& operator+=(const T& o) { return doOpDest(o, std::plus<T>()); }
		fp& operator-=(const T& o) { return doOpDest(o, std::minus<T>()); }
		fp& operator*=(const T& o) { return doOpDest(o, std::multiplies<T>()); }
		fp& operator/=(const T& o) { return doOpDest(o, std::divides<T>()); }
		fp& operator%=(const T& o) { return doOpDest(o, func::modulus<T>()); }
		fp& operator|=(const T& o) { return doOpDest(o, func::bit_or<T>()); }
		fp& operator&=(const T& o) { return doOpDest(o, func::bit_and<T>()); }
		fp& operator^=(const T& o) { return doOpDest(o, func::bit_xor<T>()); }
		fp& operator>>=(const T& o) { return doOpDest(o, [](T a, T b) {return a >> b; }); }
		fp& operator<<=(const T& o) { return doOpDest(o, [](T a, T b) {return a << b; }); }

		//Non-Destructive binary ops
		//+ - * / % | & >> <<
		template<typename BinOp, size_t... I>
		fp doOpImpl(const fp& o, BinOp op, std::index_sequence<I...>) const {
			fp ret;
			T _[] = { (DA::get(ret.data, I) =
				op(
				DA::get(data,I),
				DA::get(o.data,I)))... };
			return ret;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		fp doOp(const fp& o, BinOp op) const {
			return doOpImpl(o, op, Indices());
		}
		fp operator+(const fp& o) const { return doOp(o, std::plus<T>()); }
		fp operator-(const fp& o) const { return doOp(o, std::minus<T>()); }
		fp operator*(const fp& o) const { return doOp(o, std::multiplies<T>()); }
		fp operator/(const fp& o) const { return doOp(o, std::divides<T>()); }
		fp operator%(const fp& o) const { return doOp(o, func::modulus<T>()); }
		fp operator|(const fp& o) const { return doOp(o, func::bit_or<T>()); }
		fp operator&(const fp& o) const { return doOp(o, func::bit_and<T>()); }
		fp operator^(const fp& o) const { return doOp(o, func::bit_xor<T>()); }
		fp operator>>(const fp& o) const { return doOpDest(o, [](T a, T b) {return a >> b; }); }
		fp operator<<(const fp& o) const { return doOpDest(o, [](T a, T b) {return a << b; }); }

		//Non-Destructive singular binary ops
		//+ - * / % | & >> <<
		template<typename BinOp, size_t... I>
		fp doOpImpl(const T& o, BinOp op, std::index_sequence<I...>) const {
			fp ret;
			T _[] = { (DA::get<I>(ret.data) = op(DA::get<I>(data), o))... };
			return ret;
		}

		template<typename BinOp, typename Indices = std::make_index_sequence<N>>
		fp doOp(const T& o, BinOp op) const {
			return doOpImpl(o, op, Indices());
		}
		fp operator+(const T& o) const { return doOp(o, std::plus<T>()); }
		fp operator-(const T& o) const { return doOp(o, std::minus<T>()); }
		fp operator*(const T& o) const { return doOp(o, std::multiplies<T>()); }
		fp operator/(const T& o) const { return doOp(o, func::divides<T>()); }
		fp operator%(const T& o) const { return doOp(o, func::modulus<T>()); }
		fp operator|(const T& o) const { return doOp(o, func::bit_or<T>()); }
		fp operator&(const T& o) const { return doOp(o, func::bit_and<T>()); }
		fp operator^(const T& o) const { return doOp(o, func::bit_xor<T>()); }
		fp operator>>(const T& o) const { return doOpDest(o, [](T a, T b) {return a >> b; }); }
		fp operator<<(const T& o) const { return doOpDest(o, [](T a, T b) {return a << b; }); }

		//Unary ops
		//-~!
		template<typename UnaryOp>
		fp doOp(UnaryOp op) {
			fp res;
			for (size_t i = 0; i < N; i++) res.data[i] = UnaryOp(data[i]);
			return res;
		}
		fp operator-() { return doOp(std::negate<T>()); }
		fp operator~() { return doOp(std::bit_not<T>()); }
		fp operator!() { return doOp(std::logical_not<T>()); }

		//Comparisons
		//== != < <= > >=

		//[]
		T& operator[](size_t i) { return DA::get(data, i); }
		const T& operator[](size_t i) const { return DA::get(data, i); }
	};

	template<size_t N, typename T>
	std::ostream& operator<<(std::ostream& o, const float_base<N, T>& vec) {
		for (auto& v : vec.data) o << v << ",";
		return o;
	}

}