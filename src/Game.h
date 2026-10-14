#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include "Offsets.h"
#include "UE4.h"

namespace Game {
    // Process
    extern HANDLE hProcess;
    extern uintptr_t moduleBase;
    extern DWORD processId;

    // Global pointer'lar
    extern uintptr_t GWorld;
    extern uintptr_t GObjects;
    extern uintptr_t GNames;

    // Yerel oyuncu
    extern uintptr_t localPlayerController;
    extern uintptr_t localPawn;
    extern uintptr_t localRootComponent;
    extern uintptr_t cameraManager;

    // Fonksiyonlar
    bool Attach();
    void Detach();
    bool IsAttached();
    void ReadGlobals();
    bool ReadCamera(FMinimalViewInfo& outPov);
}