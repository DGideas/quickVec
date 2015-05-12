#pragma once

// Read the capabilities of the cpu at runtime
// Implementation heavily based on example at
// https://msdn.microsoft.com/en-us/library/hskdteyh.aspx


#include <intrin.h>
#include <string>
#include <bitset>
#include <array>
#include <vector>

class InstructionSet
{
public:
	bool SSE3(void) { return f_1_ECX_[0]; }
	bool PCLMULQDQ(void) { return f_1_ECX_[1]; }
	bool MONITOR(void) { return f_1_ECX_[3]; }
	bool SSSE3(void) { return f_1_ECX_[9]; }
	bool FMA(void) { return f_1_ECX_[12]; }
	bool CMPXCHG16B(void) { return f_1_ECX_[13]; }
	bool SSE41(void) { return f_1_ECX_[19]; }
	bool SSE42(void) { return f_1_ECX_[20]; }
	bool MOVBE(void) { return f_1_ECX_[22]; }
	bool POPCNT(void) { return f_1_ECX_[23]; }
	bool AES(void) { return f_1_ECX_[25]; }
	bool XSAVE(void) { return f_1_ECX_[26]; }
	bool OSXSAVE(void) { return f_1_ECX_[27]; }
	bool AVX(void) { return f_1_ECX_[28]; }
	bool F16C(void) { return f_1_ECX_[29]; }
	bool RDRAND(void) { return f_1_ECX_[30]; }

	bool MSR(void) { return f_1_EDX_[5]; }
	bool CX8(void) { return f_1_EDX_[8]; }
	bool SEP(void) { return f_1_EDX_[11]; }
	bool CMOV(void) { return f_1_EDX_[15]; }
	bool CLFSH(void) { return f_1_EDX_[19]; }
	bool MMX(void) { return f_1_EDX_[23]; }
	bool FXSR(void) { return f_1_EDX_[24]; }
	bool SSE(void) { return f_1_EDX_[25]; }
	bool SSE2(void) { return f_1_EDX_[26]; }

	bool FSGSBASE(void) { return f_7_EBX_[0]; }
	bool BMI1(void) { return f_7_EBX_[3]; }
	bool AVX2(void) { return f_7_EBX_[5]; }
	bool BMI2(void) { return f_7_EBX_[8]; }
	bool ERMS(void) { return f_7_EBX_[9]; }
	bool INVPCID(void) { return f_7_EBX_[10]; }
	bool AVX512F(void) { return f_7_EBX_[16]; }
	bool RDSEED(void) { return f_7_EBX_[18]; }
	bool ADX(void) { return f_7_EBX_[19]; }
	bool AVX512PF(void) { return f_7_EBX_[26]; }
	bool AVX512ER(void) { return f_7_EBX_[27]; }
	bool AVX512CD(void) { return f_7_EBX_[28]; }
	bool SHA(void) { return f_7_EBX_[29]; }

	bool PREFETCHWT1(void) { return f_7_ECX_[0]; }

	InstructionSet():
		f_1_ECX_{ 0 },
		f_1_EDX_{ 0 },
		f_7_EBX_{ 0 },
		f_7_ECX_{ 0 }
	{
		std::array<int, 4> cpui;

		// Calling __cpuid with 0x0 as the function_id argument
		// gets the number of the highest valid function ID.
		__cpuid(cpui.data(), 0);
		int nIds = cpui[0];

		if (nIds >= 1) {
			__cpuidex(cpui.data(), 1, 0);
			f_1_ECX_ = cpui[2];
			f_1_EDX_ = cpui[3];
		}

		if (nIds >= 7) {
			__cpuidex(cpui.data(), 7, 0);
			f_7_EBX_ = cpui[1];
			f_7_ECX_ = cpui[2];
		}
	};
private:
	std::bitset<32> f_1_ECX_;
	std::bitset<32> f_1_EDX_;
	std::bitset<32> f_7_EBX_;
	std::bitset<32> f_7_ECX_;
};