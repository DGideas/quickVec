#include "CppUnitTest.h"

#include "QuickVec.hpp"
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace QuickVec;

namespace QuickVecTest
{
	template<class fp, template <typename> class Op, size_t...I>
	void testOpImpl(std::index_sequence<I...>) {
		Op<fp> op;
		Op<float> op2;
		fp f1((static_cast<float>(I) + 1)...);
		fp f2((static_cast<float>(I) + fp::size)...);
		fp f3 = op(f1, f2);
		for (int i = 0; i < fp::size; i++) {
			Assert::AreEqual(op2(f1[i], f2[i]), f3[i]);
		}

	}

	template<class fp, template <typename> class Op, typename Indices = std::make_index_sequence<fp::size>::type>
	void testOp() {
		testOpImpl<fp, Op>(Indices());
	}

	template <typename fp>
	struct TestDestOp {
		template<typename A, typename B>
		static void test(A op, B opDest) {
			fp f1;
			fp f2;
			for (size_t i = 0; i < fp::size; i++) {
				f1[i] = static_cast<float>(i);
				f2[i] = static_cast<float>(i + fp::size);
			}
			fp f3 = op(f1, f2);
			opDest(f1, f2);
			for (int i = 0; i < fp::size; i++) {
				// Test for bitwise equality using uint32_t. Otherwise fail on NAN with float.
				Assert::AreEqual(reinterpret_cast<const uint32_t&>(f1[i]), reinterpret_cast<const uint32_t&>(f3[i]));
			}
		}
	};

	
	TEST_CLASS(Float4Test)
	{
	public:
		template<typename fp>
		void testConstruct() {
			//construct order check
			fp a(1.0f, 2.0f, 3.0f, 4.0f);
			Assert::AreEqual(1.0f, a[0]);
			Assert::AreEqual(2.0f, a[1]);
			Assert::AreEqual(3.0f, a[2]);
			Assert::AreEqual(4.0f, a[3]);
			//construct from singular check
			fp b(2.0f);
			Assert::AreEqual(2.0f, b[0]);
			Assert::AreEqual(2.0f, b[1]);
			Assert::AreEqual(2.0f, b[2]);
			Assert::AreEqual(2.0f, b[3]);
		}

		TEST_METHOD(TestConstruct)
		{
			testConstruct<float_base<4>>();
			testConstruct<float4_sse>();
			testConstruct<float4_sse2>();
			testConstruct<float4_sse4_1>();
		}

		template<typename fp>
		void testArithmetic() {
			testOp<fp, std::plus>();
			testOp<fp, std::minus>();
			testOp<fp, std::multiplies>();
			testOp<fp, std::divides>();
			testOp<fp, func::modulo>();
			testOp<fp, func::bit_or>();
			testOp<fp, func::bit_and>();
			testOp<fp, func::bit_xor>();
		}

		TEST_METHOD(TestArithmetic1){
			testArithmetic<float_base<4>>();
			testArithmetic<float4_sse>();
			testArithmetic<float4_sse2>();
			testArithmetic<float4_sse4_1>();
		}

		template < typename A, typename B> 
		void testSSEDestOp(A op, B opDest) {
			float4_sse f1(1.0f, -2.0f, 3.0f, 4.0f);
			float4_sse f2(2.0f, 3.0f, -4.0f, -5.0f);
			float4_sse f3 = op(f1, f2);
			opDest(f1, f2);
			for (int i = 0; i < 4; i++) {
				Assert::AreEqual(f1[i], f3[i]);
			}
		}

		template< typename fp>
		void testDestructiveArithmetic() {
			TestDestOp<fp>::test(func::plus<fp>(), [](fp& a, fp& b) { a += b; });
			TestDestOp<fp>::test(func::minus<fp>(), [](fp& a, fp& b) { a -= b; });
			TestDestOp<fp>::test(func::multiply<fp>(), [](fp& a, fp& b) { a *= b; });
			TestDestOp<fp>::test(func::divide<fp>(), [](fp& a, fp& b) { a /= b; });
			TestDestOp<fp>::test(func::modulo<fp>(), [](fp& a, fp& b) { a %= b; });
			TestDestOp<fp>::test(func::bit_or<fp>(), [](fp& a, fp& b) { a |= b; });
			TestDestOp<fp>::test(func::bit_and<fp>(), [](fp& a, fp& b) { a &= b; });
			TestDestOp<fp>::test(func::bit_xor<fp>(), [](fp& a, fp& b) { a ^= b; });
		}

		TEST_METHOD(TestDestructiveArithmeticSSE)
		{
			testDestructiveArithmetic<float_base<4>>();
			testDestructiveArithmetic<float4_sse>();
			testDestructiveArithmetic<float4_sse2>();
			testDestructiveArithmetic<float4_sse4_1>();
		}

		template< typename fp>
		void testSingularOps() {
			fp a(1, 2, 3, 4);
			fp b = a;
			a += 4;
			Assert::AreEqual(a[0], 5.0f);
			Assert::AreEqual(a[1], 6.0f);
			Assert::AreEqual(a[2], 7.0f);
			Assert::AreEqual(a[3], 8.0f);
			a -= b;
			Assert::AreEqual(a[0], 4.0f);
			Assert::AreEqual(a[1], 4.0f);
			Assert::AreEqual(a[2], 4.0f);
			Assert::AreEqual(a[3], 4.0f);
			a -= 4;
			Assert::AreEqual(a[0], 0.0f);
			Assert::AreEqual(a[1], 0.0f);
			Assert::AreEqual(a[2], 0.0f);
			Assert::AreEqual(a[3], 0.0f);
		}

		TEST_METHOD(TestSingularOps) {
			testSingularOps<float_base<4>>();
			testSingularOps<float4_sse>();
			testSingularOps<float4_sse2>();
			testSingularOps<float4_sse4_1>();
		}

		template<typename fp>
		void testModuloNeg() {
			fp a(10, -20, 30, -40);
			fp b(20, 7, -7, -6);
			fp c = a % b;
			a %= b;
			Assert::AreEqual(10.0f, c[0]);
			Assert::AreEqual(1.0f, c[1]);
			Assert::AreEqual(2.0f, c[2]);
			Assert::AreEqual(2.0f, c[3]);
		}

		TEST_METHOD(TestModuloNeg) {
			testModuloNeg<float_base<4>>();
			testModuloNeg<float4_sse>();
			testModuloNeg<float4_sse2>();
			testModuloNeg<float4_sse4_1>();
		}

		template<typename fp>
		void testUnaryOps() {
			fp a(0.0f, 1, 2, 3);
			fp b = -a;
			fp c = ~a;
			fp d = !a;
			for (int i = 0; i < 4; i++) { Assert::AreEqual(-a[i], b[i]); }
			for (int i = 0; i < 4; i++) { Assert::AreEqual(~reinterpret_cast<const int32_t&>(a[i]), reinterpret_cast<const int32_t&>(c[i])); }
			for (int i = 0; i < 4; i++) { 
				float temp = func::logical_not<float>()(a[i]);
				Assert::AreEqual(reinterpret_cast<const int32_t&>(temp), reinterpret_cast<const int32_t&>(d[i]));
			}
		}

		TEST_METHOD(TestUnaryOps) {
			testUnaryOps<float_base<4>>();
			testUnaryOps<float4_sse>();
			testUnaryOps<float4_sse2>();
			testUnaryOps<float4_sse4_1>();
		}
	};

	TEST_CLASS(Float8Test) {
	public:
		template<typename fp>
		void testConstruct() {
			//construct order check
			fp a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
			Assert::AreEqual(1.0f, a[0]);
			Assert::AreEqual(2.0f, a[1]);
			Assert::AreEqual(3.0f, a[2]);
			Assert::AreEqual(4.0f, a[3]);
			Assert::AreEqual(5.0f, a[4]);
			Assert::AreEqual(6.0f, a[5]);
			Assert::AreEqual(7.0f, a[6]);
			Assert::AreEqual(8.0f, a[7]);
			//construct from singular check
			fp b(2.0f);
			Assert::AreEqual(2.0f, b[0]);
			Assert::AreEqual(2.0f, b[1]);
			Assert::AreEqual(2.0f, b[2]);
			Assert::AreEqual(2.0f, b[3]);
			Assert::AreEqual(2.0f, b[4]);
			Assert::AreEqual(2.0f, b[5]);
			Assert::AreEqual(2.0f, b[6]);
			Assert::AreEqual(2.0f, b[7]);
		}

		TEST_METHOD(TestConstruct)
		{
			testConstruct<float_base<8>>();
			testConstruct<float8_avx>();
		}

		template<typename fp>
		void testArithmetic() {
			testOp<fp, std::plus>();
			testOp<fp, std::minus>();
			testOp<fp, std::multiplies>();
			testOp<fp, std::divides>();
			testOp<fp, func::modulo>();
		}

		TEST_METHOD(TestArithmetic1) {
			testArithmetic<float_base<8>>();
			testArithmetic<float8_avx>();
		}

		template < typename A, typename B>
		void testAVXDestOp(A op, B opDest) {
			float8_avx f1(1.0f, -2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
			float8_avx f2(2.0f, 3.0f, -4.0f, -5.0f, 6.0f, 8.0f, 9.0f, 10.0f);
			float8_avx f3 = op(f1, f2);
			opDest(f1, f2);
			for (int i = 0; i < 4; i++) {
				Assert::AreEqual(f1[i], f3[i]);
			}
		}

		template< typename fp>
		void testDestructiveArithmetic() {
			TestDestOp<fp>::test(func::plus<fp>(), [](fp& a, fp& b) { a += b; });
			TestDestOp<fp>::test(func::minus<fp>(), [](fp& a, fp& b) { a -= b; });
			TestDestOp<fp>::test(func::multiply<fp>(), [](fp& a, fp& b) { a *= b; });
			TestDestOp<fp>::test(func::divide<fp>(), [](fp& a, fp& b) { a /= b; });
			//TestDestOp<fp>::test(func::modulo<fp>(), [](fp& a, fp& b) { a %= b; });
		}

		TEST_METHOD(TestDestructiveArithmeticSSE)
		{
			testDestructiveArithmetic<float_base<8>>();
			testDestructiveArithmetic<float8_avx>();
		}

		template< typename fp>
		void testSingularOps() {
			fp a(1, 2, 3, 4, 5, 6, 7, 8);
			fp b = a;
			a += 4;
			Assert::AreEqual(a[0], 5.0f);
			Assert::AreEqual(a[1], 6.0f);
			Assert::AreEqual(a[2], 7.0f);
			Assert::AreEqual(a[3], 8.0f);
			Assert::AreEqual(a[4], 9.0f);
			Assert::AreEqual(a[5], 10.0f);
			Assert::AreEqual(a[6], 11.0f);
			Assert::AreEqual(a[7], 12.0f);
			a -= b;
			Assert::AreEqual(a[0], 4.0f);
			Assert::AreEqual(a[1], 4.0f);
			Assert::AreEqual(a[2], 4.0f);
			Assert::AreEqual(a[3], 4.0f);
			Assert::AreEqual(a[4], 4.0f);
			Assert::AreEqual(a[5], 4.0f);
			Assert::AreEqual(a[6], 4.0f);
			Assert::AreEqual(a[7], 4.0f);
			a -= 4;
			Assert::AreEqual(a[0], 0.0f);
			Assert::AreEqual(a[1], 0.0f);
			Assert::AreEqual(a[2], 0.0f);
			Assert::AreEqual(a[3], 0.0f);
			Assert::AreEqual(a[4], 0.0f);
			Assert::AreEqual(a[5], 0.0f);
			Assert::AreEqual(a[6], 0.0f);
			Assert::AreEqual(a[7], 0.0f);
		}

		TEST_METHOD(TestSingularOps) {
			testSingularOps<float_base<8>>();
			testSingularOps<float8_avx>();
		}

		template<typename fp>
		void testModuloNeg() {
			fp a(10, -20, 30, -40, 1,1,1,1);
			fp b(20, 7, -7, -6, 1,1,1,1);
			fp c = a % b;
			a %= b;
			Assert::AreEqual(10.0f, c[0]);
			Assert::AreEqual(1.0f, c[1]);
			Assert::AreEqual(2.0f, c[2]);
			Assert::AreEqual(2.0f, c[3]);
			Assert::AreEqual(0.0f, c[4]);
			Assert::AreEqual(0.0f, c[5]);
			Assert::AreEqual(0.0f, c[6]);
			Assert::AreEqual(0.0f, c[7]);
		}

		TEST_METHOD(TestModuloNeg) {
			testModuloNeg<float_base<8>>();
			testModuloNeg<float8_avx>();
		}

		template<typename fp>
		void testUnaryOps() {
			fp a(0.0f, 1, 2, 3, 4, 5, 6, 7);
			fp b = -a;
			for (int i = 0; i < 4; i++) { Assert::AreEqual(-a[i], b[i]); }
		}

		TEST_METHOD(TestUnaryOps) {
			testUnaryOps<float_base<8>>();
			testUnaryOps<float8_avx>();
		}
	};

	TEST_CLASS(Int4Test) {

		template<typename VT>
		void testConstruct() {
			//construct order check
			VT a(1, 2, 3, 4);
			Assert::AreEqual(1, a[0]);
			Assert::AreEqual(2, a[1]);
			Assert::AreEqual(3, a[2]);
			Assert::AreEqual(4, a[3]);
			//construct from singular check
			VT b(2);
			Assert::AreEqual(2, b[0]);
			Assert::AreEqual(2, b[1]);
			Assert::AreEqual(2, b[2]);
			Assert::AreEqual(2, b[3]);
		}

		TEST_METHOD(TestConstruct) {
			testConstruct<int32_base<4>>();
			testConstruct<int32x4_sse2>();
			testConstruct<int32x4_sse4_1>();
		}

		template<typename VT,template <typename...> typename Op>
		void testOp() {
			Op<VT> opV;
			Op<int> opT;
			VT v(1, 2, 3, 4);
			VT v2(1, 2, 3, 4);
			VT res = opV(v, v2);
			for (uint32_t i = 0; i < 4; i++) {
				int resT = opT(v[i], v2[i]);
				Assert::AreEqual(resT, res[i]);
			}
		}

		template<typename VT>
		void testBinaryArithmetic() {
			testOp<VT, func::plus>();
			testOp<VT, func::minus>();
			testOp<VT, func::multiply>();
			testOp<VT, func::divide>();
			testOp<VT, func::modulo>();
			testOp<VT, func::bit_or>();
			testOp<VT, func::bit_and>();
			testOp<VT, func::bit_xor>();
			testOp<VT, func::shift_left>();
			testOp<VT, func::shift_right>();
		}

		TEST_METHOD(TestBinaryArithmetic) {
			testBinaryArithmetic<int32_base<4>>();
			testBinaryArithmetic<int32x4_sse2>();
			testBinaryArithmetic<int32x4_sse4_1>();
		}

		template<typename VT>
		bool equal(const VT& a, const VT& b) {
			for (uint32_t i = 0; i < 4; i++) {
				if (a[i] != b[i]) return false;
			}
			return true;
		}

		template<typename VT>
		void testDestructiveBinaryArithmetic() {
			VT v(1, 2, 3, 4);
			VT a = v;
			VT b = v;
			Assert::IsTrue(equal(b += 1, a + 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b -= 1, a - 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b /= 1, a / 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b *= 1, a * 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b %= 1, a % 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b |= 1, a | 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b &= 1, a & 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b ^= 1, a ^ 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b <<= 1, a << 1));
			a = v;
			b = v;
			Assert::IsTrue(equal(b >>= 1, a >> 1));
		}

		TEST_METHOD(TestDestructiveBinaryArithmetic) {
			testDestructiveBinaryArithmetic<int32_base<4>>();
			testDestructiveBinaryArithmetic<int32x4_sse2>();
			testDestructiveBinaryArithmetic<int32x4_sse4_1>();
		}

		template<typename VT>
		void testUnaryArithmetic() {
			VT a(1, 2, 3, 4);
			VT b = -a;
			VT c = ~a;
			VT d = !a;
			for (int i = 0; i < 4; i++) { Assert::AreEqual(-a[i], b[i]); }
			for (int i = 0; i < 4; i++) { Assert::AreEqual(~a[i], c[i]); }
			for (int i = 0; i < 4; i++) { Assert::AreEqual((!a[i])?-1:0, d[i]); }
		}

		TEST_METHOD(TestUnaryArithmetic) {
			testUnaryArithmetic<int32_base<4>>();
			testUnaryArithmetic<int32x4_sse2>();
			testUnaryArithmetic<int32x4_sse4_1>();
		}

		template<typename VT>
		void testSingular() {
			VT a = 1;
			for (int i = 0; i < 4; i++) { Assert::AreEqual(1, a[i]); }
		}

		TEST_METHOD(TestSingular) {
			testSingular<int32_base<4>>();
			testSingular<int32x4_sse2>();
			testSingular<int32x4_sse4_1>();
		}
	};

}