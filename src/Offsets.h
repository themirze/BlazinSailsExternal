#pragma once
#include <cstdint>

namespace Offsets {
    // Global
    constexpr uintptr_t GWorld = 0x51A4738;
    constexpr uintptr_t GNames = 0x5022C80;
    constexpr uintptr_t GObjects = 0x505F300;

    // UWorld
    constexpr uintptr_t UWorld_OwningGameInstance = 0x180;

    // UGameInstance
    constexpr uintptr_t GameInstance_LocalPlayers = 0x38;

    // UPlayer
    constexpr uintptr_t LocalPlayer_PlayerController = 0x30;

    // APlayerController
    constexpr uintptr_t PC_AcknowledgedPawn = 0x2A0;
    constexpr uintptr_t PC_PlayerCameraManager = 0x2B8;

    // AActor
    constexpr uintptr_t Actor_RootComponent = 0x130;

    // USceneComponent
    constexpr uintptr_t Scene_RelativeLocation = 0x11C;

    // BP_Character_C
    constexpr uintptr_t Char_UserName = 0x648;
    constexpr uintptr_t Char_Mesh = 0x280;
    constexpr uintptr_t Char_Health = 0x760;
    constexpr uintptr_t Char_MaxHealth = 0x764;
    constexpr uintptr_t Char_IsDead = 0x8F6;
    constexpr uintptr_t Char_IsEnemy = 0xCA0;
}
