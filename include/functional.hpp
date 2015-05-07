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
		struct modulo {
			T operator()(const T& a, const T& b) const {
				return a % b;
			}
		};

		template <>
		struct modulo<float> {
			using T = float;
			//Always positive
			T operator()(const T& a, const T& b) const {
				return (a >= 0.0f) ? fmod(a, b) : (b >= 0.0f ) ? b + fmod(a, b): -b + fmod(a, b);
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
				return reinterpret_cast<const float&>(r);
			}
		};

		template<typename T>
		struct negate {
			T operator()(const T& a) {
				return -a;
			}
		};

		template<typename T>
		struct bit_not {
			T operator()(const T& a) {
				return ~a;
			}
		};

		template<>
		struct bit_not<float> {
			using T = float;
			T operator()(const T& a) {
				uint32_t r = ~reinterpret_cast<const uint32_t&>(a);
				return reinterpret_cast<const float&>(r);
			}
		};

		template<typename T>
		struct logical_not {
			T operator()(const T& a) {
				return !a;
			}
		};

		template<>
		struct logical_not<int> {
			using T = int;
			T operator()(const T& a) {
				return (!a)?-1:0;
			}
		};

		template<>
		struct logical_not<float> {
			using T = float;
			T operator()(const T& a) {
				uint32_t r = reinterpret_cast<const uint32_t&>(a);
				uint32_t x = (!r) ? -1: 0;
				return reinterpret_cast<const float&>(x);
			}
		};

		template<typename T>
		struct shift_right {
			T operator()(const T& a, const T& b) {
				return a >> b;
			}
		};

		template<typename T>
		struct shift_left {
			T operator()(const T& a, const T& b) {
				return a << b;
			}
		};

	
	}
}