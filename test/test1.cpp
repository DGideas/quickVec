#include "CppUnitTest.h"

#include "QuickVec.hpp"
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace QuickVec;

namespace QuickVecTest
{

	template<typename T>
	int areEqualWrapper(const T& a, const T& b) {
		Assert::AreEqual(a, b);
		return 0;
	}
	template<class fp, size_t...I> 
	void testConstructImpl(std::index_sequence<I...>) {
		fp f((static_cast<float>(I))...);
		int _[] = {
			(areEqualWrapper(static_cast<float>(I), f[I]))...
		};
	}

	template<class fp, typename Indices = std::make_index_sequence<fp::Size>::type>
	void testConstruct() {
		testConstructImpl<fp>(Indices());
	}

	template<class fp, template <typename> class Op, size_t...I>
	void testOpImpl(std::index_sequence<I...>) {
		Op<fp> op;
		Op<float> op2;
		fp f1((static_cast<float>(I)+1)...);
		fp f2((static_cast<float>(I)+fp::Size)...);
		fp f3 = op(f1, f2);
		for (int i = 0; i < fp::Size; i++) {
			Assert::AreEqual(op2(f1[i], f2[i]), f3[i]);
		}

	}

	template<class fp, template <typename> class Op, typename Indices = std::make_index_sequence<fp::Size>::type>
	void testOp() {
		testOpImpl<fp, Op>(Indices());
	}

	template <typename fp>
	struct TestDestOp{
		template<typename A, typename B>
		static void test(A op, B opDest) {
			fp f1;
			fp f2;
			for (size_t i = 0; i < fp::Size; i++) {
				f1[i] = static_cast<float>(i);
				f2[i] = static_cast<float>(i + fp::Size);
			}
			fp f3 = op(f1, f2);
			opDest(f1, f2);
			for (int i = 0; i < fp::Size; i++) {
				// Test for bitwise equality using uint32_t. Otherwise fail on NAN with float.
				Assert::AreEqual(reinterpret_cast<const uint32_t&>(f1[i]), reinterpret_cast<const uint32_t&>(f3[i]));
			}
		}
	};

	TEST_CLASS(SequentialTest1)
	{
	public:
		
		TEST_METHOD(TestConstruct1)
		{
			testConstruct<float_base<4, float>>();
		}

		TEST_METHOD(TestArithmetic1)
		{
			testOp<float_base<4, float>, std::plus>();
			testOp<float_base<4, float>, std::minus>();
			testOp<float_base<4, float>, std::multiplies>();
			testOp<float_base<4, float>, std::divides>();
			testOp<float_base<4, float>, func::modulus>();
			testOp<float_base<4, float>, func::bit_or>();
			testOp<float_base<4, float>, func::bit_and>();
			testOp<float_base<4, float>, func::bit_xor>();
		}

		TEST_METHOD(TestDestructiveArithmetic)
		{
			using fp = float_base<4, float>;
			TestDestOp<fp>::test(std::plus<fp>(), [](fp& a, fp& b) { a += b; });
			TestDestOp<fp>::test(std::minus<fp>(), [](fp& a, fp& b) { a -= b; });
			TestDestOp<fp>::test(std::multiplies<fp>(), [](fp& a, fp& b) { a *= b; });
			TestDestOp<fp>::test(std::divides<fp>(), [](fp& a, fp& b) { a /= b; });
			TestDestOp<fp>::test(func::modulus<fp>(), [](fp& a, fp& b) { a %= b; });
			TestDestOp<fp>::test(func::bit_or<fp>(), [](fp& a, fp& b) { a |= b; });
			TestDestOp<fp>::test(func::bit_and<fp>(), [](fp& a, fp& b) { a &= b; });
			TestDestOp<fp>::test(func::bit_xor<fp>(), [](fp& a, fp& b) { a ^= b; });
		}
	};

	TEST_CLASS(SSETest1)
	{
	public:

		TEST_METHOD(TestConstruct1)
		{
			testConstruct<float_sse>();
		}

		TEST_METHOD(TestArithmetic1)
		{
			testOp<float_sse, std::plus>();
			testOp<float_sse, std::minus>();
			testOp<float_sse, std::multiplies>();
			testOp<float_sse, std::divides>();
			testOp<float_sse, func::modulus>();
			testOp<float_sse, func::bit_or>();
			testOp<float_sse, func::bit_and>();
			testOp<float_sse, func::bit_xor>();
		}

		template < typename A, typename B> 
		void testSSEDestOp(A op, B opDest) {
			float_sse f1(1.0f, 2.0f, 3.0f, 4.0f);
			float_sse f2(2.0f, 3.0f, 4.0f, 5.0f);
			float_sse f3 = op(f1, f2);
			opDest(f1, f2);
			for (int i = 0; i < 4; i++) {
				Assert::AreEqual(f1[i], f3[i]);
			}
		}

		TEST_METHOD(TestDestructiveArithmetic)
		{
			using fp = float_sse;
			TestDestOp<fp>::test(std::plus<fp>(), [](float_sse& a, float_sse& b) { a += b; });
			TestDestOp<fp>::test(std::minus<fp>(), [](float_sse& a, float_sse& b) { a -= b; });
			TestDestOp<fp>::test(std::multiplies<fp>(), [](float_sse& a, float_sse& b) { a *= b; });
			TestDestOp<fp>::test(std::divides<fp>(), [](float_sse& a, float_sse& b) { a /= b; });
		}
	};
}