#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
DWORD GetProcId(const wchar_t* windowTitle);
