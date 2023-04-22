#include "mem.h"

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}

void mem::PatchEx(BYTE* dstAddr, BYTE* srcAddr, unsigned int size, HANDLE hProcess)
{
	DWORD oldProtectionRight;
	VirtualProtectEx(hProcess, dstAddr, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	WriteProcessMemory(hProcess, dstAddr, srcAddr, size, nullptr);
	VirtualProtectEx(hProcess, dstAddr, size, oldProtectionRight, &oldProtectionRight);
}
void mem::PatchExVal(BYTE* dstAddr, int val, unsigned int size, HANDLE hProcess)
{
	DWORD oldProtectionRight;
	VirtualProtectEx(hProcess, dstAddr, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	WriteProcessMemory(hProcess, dstAddr, (BYTE*)&val, size, nullptr);
	VirtualProtectEx(hProcess, dstAddr, size, oldProtectionRight, &oldProtectionRight);
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

mem::MemPatchMem mem::PatchWithMem(BYTE* dstAddr, BYTE* srcAddr, unsigned int size)
{
	MemPatchMem returnVal;
	DWORD oldProtectionRight;
	VirtualProtect(dstAddr, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	
	//Copy old data to Mem
	memcpy(returnVal.data, dstAddr, size);
	returnVal.dstAddr = dstAddr; 
	returnVal.size = size;

	memcpy(dstAddr, srcAddr, size);
	VirtualProtect(dstAddr, size, oldProtectionRight, &oldProtectionRight);
	return returnVal;
}
void mem::RestoreFromMem(mem::MemPatchMem mem)
{
	DWORD oldProtectionRight;
	VirtualProtect(mem.dstAddr, mem.size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	memcpy(mem.dstAddr, mem.data, mem.size);
	VirtualProtect(mem.dstAddr, mem.size, oldProtectionRight, &oldProtectionRight);
}

void mem::Patch(BYTE* dstAddr, BYTE* srcAddr, unsigned int size)
{
	DWORD oldProtectionRight;
	VirtualProtect(dstAddr, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	memcpy(dstAddr, srcAddr, size);
	VirtualProtect(dstAddr, size, oldProtectionRight, &oldProtectionRight);
}
void  mem::PatchVal(BYTE* dstAddr, int val, unsigned int size)
{
	DWORD oldProtectionRight;
	VirtualProtect(dstAddr, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	memcpy(dstAddr, (BYTE*)&val, size);
	VirtualProtect(dstAddr, size, oldProtectionRight, &oldProtectionRight);
}

void mem::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldProtectionRight;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtectionRight, &oldProtectionRight);

}
void mem::NopMips(BYTE* dst, unsigned int instructions)
{
	int size = instructions * 4;
	DWORD oldProtectionRight;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtectionRight);
	memset(dst, 0x00, size);
	VirtualProtect(dst, size, oldProtectionRight, &oldProtectionRight);

}


void mem::MoveBytesMips(BYTE* srcAddress, BYTE* dstAddress, unsigned int size)
{
	DWORD oldProtectionRightSrc;
	DWORD oldProtectionRightDst;
	VirtualProtect(srcAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightSrc);
	VirtualProtect(dstAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightDst);
	memcpy( dstAddress, srcAddress, size);
	NopMips(srcAddress, size / 4);

	VirtualProtect(srcAddress, size, oldProtectionRightSrc, &oldProtectionRightSrc);
	VirtualProtect(dstAddress, size, oldProtectionRightDst, &oldProtectionRightDst);

}

void mem::ShiftBytesMips(BYTE* srcAddress, BYTE* dstAddress, unsigned int size)
{
	DWORD oldProtectionRightSrc;
	DWORD oldProtectionRightDst;
	int distance = *(int*)dstAddress - *(int*)srcAddress;
	VirtualProtect(srcAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightSrc);
	VirtualProtect(dstAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightDst);
	memcpy(dstAddress, srcAddress, size);
	if (distance > 0)
	{
		NopMips(srcAddress, distance / 4);
	}
	if (distance < 0 && size > abs(distance))
	{
		int difference = size - abs(distance);
		NopMips(srcAddress + difference, distance / 4);
	}
	else
	{
		NopMips(srcAddress, size / 4);
	}	
	VirtualProtect(srcAddress, size, oldProtectionRightSrc, &oldProtectionRightSrc);
	VirtualProtect(dstAddress, size, oldProtectionRightDst, &oldProtectionRightDst);

}
void mem::CopyBytes(BYTE* srcAddress, BYTE* dstAddress, unsigned int size)
{
	DWORD oldProtectionRightDst;
	VirtualProtect(dstAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightDst);
	memcpy(dstAddress, srcAddress, size);
	VirtualProtect(dstAddress, size, oldProtectionRightDst, &oldProtectionRightDst);

}
void mem::MoveBytes(BYTE* srcAddress, BYTE* dstAddress, unsigned int size)
{
	DWORD oldProtectionRightSrc;
	DWORD oldProtectionRightDst;
	VirtualProtect(srcAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightSrc);
	VirtualProtect(dstAddress, size, PAGE_EXECUTE_READWRITE, &oldProtectionRightDst);
	memcpy(dstAddress,srcAddress, size);
	mem::Nop(srcAddress, size);
	VirtualProtect(srcAddress, size, oldProtectionRightSrc, &oldProtectionRightSrc);
	VirtualProtect(dstAddress, size, oldProtectionRightDst, &oldProtectionRightDst);

}

