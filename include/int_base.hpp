#pragma once

#include <array>
#include <iostream>
#include "numeric_base.hpp"

namespace QuickVec {

	template<size_t N, typename data_t = std::array<int,N> >
	using int32_base = numeric_base<int, N, data_t>;
}