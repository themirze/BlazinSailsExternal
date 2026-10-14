#pragma once
#include <cstdint>

namespace Offsets {
    // Global
    constexpr uintptr_t GWorld = 0x51A4738;
    constexpr uintptr_t GNames = 0x5022C80;
    constexpr uintptr_t GObjects = 0x505F300;

    // UWorld
    constexpr uintptr_t UWorld_OwningGameInstance = 0x180;
    constexpr uintptr_t UWorld_PersistentLevel = 0x30;

    // UGameInstance
    constexpr uintptr_t GameInstance_LocalPlayers = 0x38;

    // UPlayer
    constexpr uintptr_t LocalPlayer_PlayerController = 0x30;

    // APlayerController
    constexpr uintptr_t PC_AcknowledgedPawn = 0x2A0;
    constexpr uintptr_t PC_PlayerCameraManager = 0x2B8;

    // APlayerCameraManager
    constexpr uintptr_t PCM_CameraCachePrivate = 0x1AE0;

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

    // ULevel
    constexpr uintptr_t ULevel_Actors = 0x98;

    // Mesh ComponentToWorld Translation
    constexpr uintptr_t Mesh_ComponentToWorld_Translation = 0x100;
}

namespace ItemClasses {
    // Silahlar
    constexpr int32_t Flintlock = 708614;
    constexpr int32_t Blunderbuss = 861990;
    constexpr int32_t Crossbow = 795005;
    constexpr int32_t Musket = 985456;
    constexpr int32_t Club = 1097798;
    constexpr int32_t Repeater = 1232595;
    constexpr int32_t DoubleBarrel = 1045948;
    constexpr int32_t HealingCrossbow = 755857;
    constexpr int32_t Cutlass = 1232541;
    constexpr int32_t Sniper = 941874;
    constexpr int32_t Pickaxe = 473640;
    constexpr int32_t Shovel = 1263863;
    constexpr int32_t ButchersKnife = 1201661;
    constexpr int32_t DualFlintlock = 1057728;
    constexpr int32_t QuadFlintlock = 893114;

    // Sandıklar
    constexpr int32_t Lootcrate = 1188136;
    constexpr int32_t ExplosiveBarrel = 802931;
    constexpr int32_t TreasureChest = 802858;
    constexpr int32_t AmmoBox = 440564;

    // Mermiler
    constexpr int32_t SmallShot = 821509;
    constexpr int32_t Arrows = 1191514;
    constexpr int32_t CannonBalls = 981160;

    // Gemi
    constexpr int32_t VehicleCannon = 609612;
    constexpr int32_t SwivelCannon = 494796;
}