#pragma once

#include <array>
#include <iostream>
#include "array_accessor.hpp"
#include "numeric_base.hpp"

namespace QuickVec {

	template<size_t N, typename Data_t = typename std::array<int, N>, typename DataAccessor = array_accessor<int, N>>
	using int32_base = numeric_base<N, int, Data_t, DataAccessor>;
}