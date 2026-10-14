#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

namespace Memory {
    extern HANDLE hProcess;   // ← extern olmalı!

    template<typename T>
    T Read(uintptr_t address) {
        T buffer{};
        if (address == 0 || hProcess == nullptr) return buffer;
        ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), nullptr);
        return buffer;
    }
}