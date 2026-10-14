#pragma once

namespace Config {
    inline bool menuOpen = true;
    inline bool espEnabled = true;

    // ===== KARAKTER ESP =====
    inline bool espCharacters = true;
    inline bool espBox = false;
    inline bool espName = true;
    inline bool espDistance = true;
    inline bool espHealth = true;
    inline bool espTeam = false;
    inline bool espDead = false;
    inline float espMaxDistance = 500.0f;

    // ===== ITEM ESP =====
    inline bool espItems = true;
    inline float espItemMaxDistance = 300.0f;
    inline bool itemShowChests = false;
    inline bool itemShowWeapons = true;
    inline bool itemShowAmmo = true;
    inline bool itemShowBarrels = false;
    inline bool itemShowUpgrades = true;
    inline bool itemShowCannons = false;
    inline bool itemShowResources = false;
    inline bool itemShowPaddles = false;
    inline bool itemShowOther = false;

    // ===== GÖRSEL =====
    inline float espBoxThickness = 1.5f;
    inline float colorEnemy[4] = { 1.0f, 0.2f, 0.2f, 1.0f };
    inline float colorTeam[4] = { 0.2f, 1.0f, 0.2f, 1.0f };
    inline float colorText[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // ===== AIMLOCK =====
    inline bool aimlockEnabled = false;
    inline bool aimlockOnRightMouse = true;
    inline bool aimlockOnLeftMouse = false;
    inline bool aimlockOnCtrl = false;
    inline bool aimlockOnShift = false;
    inline bool aimlockOnAlt = false;
    inline float aimlockSmooth = 5.0f;      // 1 = instant, 20 = çok yumuşak
    inline float aimlockMaxStep = 15.0f;    // max piksel/frame

    // ===== MAGIC BULLET =====
    inline bool magicEnabled = false;
    inline bool magicOnLeftMouse = true;
    inline bool magicOnRightMouse = false;
    inline float magicSmooth = 1.0f;

    // ===== Ortak aim =====
    inline bool aimDrawFOV = true;
    inline float aimFOV = 120.0f;
    inline bool aimTeam = false;
    inline int aimBone = 0;                 // 0=Kafa 1=Göğüs 2=Ayak
    inline float aimHeadOffset = 85.0f;
    inline float aimChestOffset = 45.0f;
    inline float aimFootOffset = -80.0f;

    // ===== TRIGGERBOT =====
    inline bool triggerEnabled = false;
    inline bool triggerOnRightMouse = true; // Sağ tık (ADS) → aktif
    inline bool triggerOnCtrl = false;
    inline bool triggerOnShift = false;
    inline int triggerDelayMs = 0;
    inline float triggerFOV = 8.0f;
    inline bool triggerHeadOnly = false;

    // ===== Write-based magic bullet =====
    inline bool magicWriteEnabled = false;
    inline int magicWriteCooldownMs = 500;
}
