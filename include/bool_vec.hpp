// bool_vec.hpp
// Copyright 2015 Matthew Kellogg

// This defines the base template for boolean vectors
// Implementations must implement the following
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

#pragma once

#include <xmmintrin.h>

//TODO: a splattering of noexcept

namespace QuickVec {
	template<size_t N>
	class bool_seq {
		using bool_t = bool_base<N, data_t>;
		using data_t = std::array<bool, N>;
		data_t raw;
	public:
		
		bool_t() = default;
		bool_t(bool_t&& o) = default;
		bool_t(const bool_t& o) = default;

		template <typename... Args, typename Enable = std::enable_if<sizeof...(Args) == N - 1>::type>
		bool_t(bool&& t, Args&&... args) : raw({ t, static_cast<bool>(args)... }) {};

		bool_t(const bool& b) {
			for (size_t i = 0; i < N; i++) {
				raw[i] = b;
			}
		}

		explicit bool_t(const data_t& o) : raw(o) {};

		bool all() const {
			for (int i = 0; i < N; i++) {
				if (!raw[i]) return false;
			}
			return true;
		}

		bool any() const {
			for (int i = 0; i < N; i++) {
				if (raw[i]) return true;
			}
			return false;
		}

		//[]
		bool& operator[](size_t i) { return raw[i]; }
		const bool& operator[](size_t i) const { return raw[i]; }
	};

	/*

	template<size_t size, typename data_t = std::array<bool,size> >
	class alignas(data_t)bool_vec
	{
		using bool_t = bool_vec<size, data_t>;
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
	};*/

	template<size_t size, typename data_t>
	inline bool all(const bool_vec<size, data_t>& b) {
		return b.all();
	}

	template<size_t size, typename data_t>
	inline bool any(const bool_vec<size, data_t>& b) {
		return b.all();
	}
}