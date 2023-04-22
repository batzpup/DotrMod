#pragma once

#include <Windows.h>

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);
void UnHookTramp32(BYTE* gateway, BYTE* originalFunc, const uintptr_t len);