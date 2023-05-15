#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
namespace mem
{
	struct MemPatchMem
	{
		BYTE* dstAddr;
		unsigned char data[1000];
		unsigned int size;

	};
	void PatchEx(BYTE* dstAddr, BYTE* srcAddr, unsigned int size, HANDLE hProcess);
	void PatchExVal(BYTE* dstAddr, int val, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	void MoveBytes(BYTE* srcAddress, BYTE* dstAddress, unsigned int size);
	void MoveBytesMips(BYTE* srcAddress, BYTE* dstAddress, unsigned int size);
	void ShiftBytesMips(BYTE* srcAddress, BYTE* dstAddress, unsigned int size);
	void ShiftBytesMips(BYTE* srcAddress,int numberOfInstructions);
	void CopyBytes(BYTE* srcAddress, BYTE* dstAddress, unsigned int size);
	void Patch(BYTE* dstAddr, BYTE* srcAddr, unsigned int size);
	void PatchVal(BYTE* dstAddr, int val, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	void NopMips(BYTE* dst, unsigned int size);
	MemPatchMem PatchWithMem(BYTE* dstAddr, BYTE* srcAddr, unsigned int size);
	void RestoreFromMem(mem::MemPatchMem mem);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
}
