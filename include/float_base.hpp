#pragma once

#include <array>
#include <iostream>
#include "numeric_base.hpp"

namespace QuickVec {

	template <uint32_t N, typename data_t = std::array<float, N> >
	using float_base = numeric_base<float, N, data_t>;
}