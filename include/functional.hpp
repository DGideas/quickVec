#include <functional>

namespace QuickVec {

	namespace func {

		template<typename T>
		using func::plus = std::plus<T>;

		template<typename T>
		using func::minus = std::minus<T>;

		template<typename T>
		using func::multiply = std::multiplies<T>;

		template<typename T>
		using func::divide = std::divides<T>;

		template <typename T>
		struct modulus {
			T operator()(const T& a, const T& b) const {
				return a % b;
			}
		};

		template <>
		struct modulus<float> {
			using T = float;
			T operator()(const T& a, const T& b) const {
				return fmod(a, b);
			}
		};

		template <typename T>
		struct bit_or {
			T operator()(const T& a, const T& b) const {
				return a | b;
			}
		};

		template <>
		struct bit_or<float> {
			using T = float;
			T operator()(const T& a, const T& b) const {
				uint32_t r = (reinterpret_cast<const uint32_t&>(a) | reinterpret_cast<const uint32_t&>(b));
				return reinterpret_cast<float&>(r);
			}
		};

		template <typename T>
		struct bit_and {
			T operator()(const T& a, const T& b) const {
				return a & b;
			}
		};

		template <>
		struct bit_and<float> {
			using T = float;
			T operator()(const T& a, const T& b) const {
				uint32_t r = (reinterpret_cast<const uint32_t&>(a) & reinterpret_cast<const uint32_t&>(b));
				return reinterpret_cast<float&>(r);
			}
		};

		template <typename T>
		struct bit_xor {
			T operator()(const T& a, const T& b) const {
				return a ^ b;
			}
		};

		template <>
		struct bit_xor<float> {
			using T = float;
			T operator()(const T& a, const T& b) const {
				uint32_t r = (reinterpret_cast<const uint32_t&>(a) ^ reinterpret_cast<const uint32_t&>(b));
				return reinterpret_cast<float&>(r);
			}
		};

		template <typename T>
		T negate(const T& a) {
			return -a;
		}

		template <typename T>
		T bit_not(const T& a) {
			return ~a;
		}

		template <typename T>
		T logical_not(const T& a) {
			return !a;
		}
	}
}