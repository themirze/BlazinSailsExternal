#include "Game.h"
#include "Memory.h"
#include <TlHelp32.h>
#include <cstdio>
#include <cmath>

namespace Game {

    uintptr_t moduleBase = 0;
    DWORD processId = 0;
    uintptr_t GWorld = 0, GObjects = 0, GNames = 0;
    uintptr_t localPlayerController = 0;
    uintptr_t localPawn = 0;
    uintptr_t localRootComponent = 0;
    uintptr_t cameraManager = 0;

    // ===== TimeStamp cache =====
    static float s_lastTimeStamp = -1.0f;
    static FMinimalViewInfo s_cachedPov = {};

    bool Attach() {
        if (Memory::hProcess) return true;
        const wchar_t* procName = L"BlazingSails-Win64-Shipping.exe";

        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return false;

        PROCESSENTRY32W pe = { sizeof(pe) };
        bool found = false;
        if (Process32FirstW(snap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, procName) == 0) {
                    processId = pe.th32ProcessID;
                    found = true;
                    break;
                }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
        if (!found) return false;

        Memory::hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (!Memory::hProcess) return false;

        HANDLE modSnap = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
        if (modSnap != INVALID_HANDLE_VALUE) {
            MODULEENTRY32W me = { sizeof(me) };
            if (Module32FirstW(modSnap, &me)) {
                do {
                    if (_wcsicmp(me.szModule, procName) == 0) {
                        moduleBase = (uintptr_t)me.modBaseAddr;
                        break;
                    }
                } while (Module32NextW(modSnap, &me));
            }
            CloseHandle(modSnap);
        }

        printf("[Game] Attached. PID=%lu Base=0x%llX\n",
            (unsigned long)processId, (unsigned long long)moduleBase);
        return true;
    }

    void Detach() {
        if (Memory::hProcess) {
            CloseHandle(Memory::hProcess);
            Memory::hProcess = nullptr;
        }
        moduleBase = 0;
        GWorld = GObjects = GNames = 0;
        localPlayerController = localPawn = 0;
        localRootComponent = cameraManager = 0;

        // Cache sıfırla
        s_lastTimeStamp = -1.0f;
        s_cachedPov = {};
    }

    bool IsAttached() {
        return Memory::hProcess != nullptr && moduleBase != 0;
    }

    void ReadGlobals() {
        if (!IsAttached()) return;
        GWorld = Memory::Read<uintptr_t>(moduleBase + Offsets::GWorld);
        GObjects = Memory::Read<uintptr_t>(moduleBase + Offsets::GObjects);
        GNames = moduleBase + Offsets::GNames;
    }

    bool ReadCamera(FMinimalViewInfo& outPov) {
        if (!GWorld) return false;

        uintptr_t gameInstance = Memory::Read<uintptr_t>(
            GWorld + Offsets::UWorld_OwningGameInstance);
        if (!gameInstance) return false;

        uintptr_t localPlayersPtr = Memory::Read<uintptr_t>(
            gameInstance + Offsets::GameInstance_LocalPlayers);
        if (!localPlayersPtr) return false;

        uintptr_t localPlayer = Memory::Read<uintptr_t>(localPlayersPtr);
        if (!localPlayer) return false;

        uintptr_t pc = Memory::Read<uintptr_t>(
            localPlayer + Offsets::LocalPlayer_PlayerController);
        if (!pc) return false;
        localPlayerController = pc;

        uintptr_t pcm = Memory::Read<uintptr_t>(
            pc + Offsets::PC_PlayerCameraManager);
        if (!pcm) return false;
        cameraManager = pcm;

        // ===== 0x20E0 (LastFrameCameraCachePrivate) =====
        // Render pipeline gecikmesini telafi eder
        uintptr_t cacheAddr = pcm + 0x20E0;
        float timeStamp = Memory::Read<float>(cacheAddr + 0x00);

        // TimeStamp değiştiyse yeni frame geldi
        if (timeStamp != s_lastTimeStamp) {

            FMinimalViewInfo curr = Memory::Read<FMinimalViewInfo>(cacheAddr + 0x10);

            // Velocity ekstrapolasyon
            if (s_lastTimeStamp > 0.0f) {
                float vx = curr.Location.X - s_cachedPov.Location.X;
                float vy = curr.Location.Y - s_cachedPov.Location.Y;
                float vz = curr.Location.Z - s_cachedPov.Location.Z;

                // Ani zıplamaları filtrele
                if (fabsf(vx) < 300.0f && fabsf(vy) < 300.0f && fabsf(vz) < 300.0f) {
                    // Hafif ileri tahmin (0.3 = güvenli)
                    curr.Location.X += vx * 0.3f;
                    curr.Location.Y += vy * 0.3f;
                    curr.Location.Z += vz * 0.3f;
                }
            }

            s_cachedPov = curr;
            s_lastTimeStamp = timeStamp;
        }

        // Cache'lenmiş POV'u dön
        outPov = s_cachedPov;

        // LocalPawn güncelle
        localPawn = Memory::Read<uintptr_t>(pc + Offsets::PC_AcknowledgedPawn);
        if (localPawn) {
            localRootComponent = Memory::Read<uintptr_t>(
                localPawn + Offsets::Actor_RootComponent);
        }

        return true;
    }
}