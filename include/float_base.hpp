#pragma once

#include <array>
#include <iostream>

namespace QuickVec {

	template <uint32_t N, typename Data_t = std::array<float, N>, typename DataAccessor = array_accessor<float, N> >
	using float_base = numeric_base<N, float, Data_t, DataAccessor>;
}