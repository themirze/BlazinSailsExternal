#include "MagicBullet.h"
#include "Config.h"
#include "Game.h"
#include "Memory.h"
#include "Offsets.h"
#include "UE4.h"
#include "Actors.h"
#include <Windows.h>
#include <cmath>

namespace MagicBullet {

    static DWORD s_lastWrite = 0;
    static bool s_prevFire = false;

    static bool IsFireDown() {
        return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    }

    // WriteProcessMemory kullanımı anti-cheat tarafından tespit edilebilir.
    static bool WriteFVector(uintptr_t address, const FVector& v) {
        if (!Memory::hProcess) return false;
        SIZE_T written = 0;
        BOOL ok = WriteProcessMemory(
            Memory::hProcess,
            (LPVOID)address,
            &v, sizeof(FVector),
            &written);
        return ok && written == sizeof(FVector);
    }

    // Hedefi bul
    static bool FindTarget(
        const std::vector<Actors::Character>& chars,
        const FMinimalViewInfo& pov,
        FVector& outTarget,
        uintptr_t& outActor)
    {
        int sw = (int)ImGui::GetIO().DisplaySize.x;
        int sh = (int)ImGui::GetIO().DisplaySize.y;
        float cx = sw * 0.5f;
        float cy = sh * 0.5f;

        float bestDist = Config::aimFOV;
        bool found = false;

        for (const auto& ch : chars) {
            if (!ch.rootComponent) continue;
            if (ch.isDead || ch.isLocalPlayer) continue;
            if (!Config::aimTeam && !ch.isEnemy) continue;

            FVector pos = Memory::Read<FVector>(
                ch.rootComponent + Offsets::Scene_RelativeLocation);

            uintptr_t mesh = Memory::Read<uintptr_t>(
                ch.actor + Offsets::Char_Mesh);
            if (mesh) {
                FVector mp = Memory::Read<FVector>(mesh + 0x100);
                if (mp.X != 0.0f && mp.Y != 0.0f && fabsf(mp.X) < 10000000.0f)
                    pos = mp;
            }

            pos.Z += Config::aimHeadOffset;

            ImVec2 sp;
            if (!UE4::WorldToScreen(pos, sp, pov, sw, sh)) continue;

            float dx = sp.x - cx;
            float dy = sp.y - cy;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist < bestDist) {
                bestDist = dist;
                outTarget = pos;
                outActor = ch.actor;
                found = true;
            }
        }
        return found;
    }

    // Yeni ateş algılanınca projectile velocity'sini hedefe yönlendir
    bool Update() {
        if (!Config::magicWriteEnabled) return false;
        if (!Game::IsAttached() || !Game::GWorld) return false;

        // EDGE DETECTION: Yeni ateş
        bool fireDown = IsFireDown();
        if (!fireDown || s_prevFire) {
            s_prevFire = fireDown;
            return false;
        }
        s_prevFire = true;

        // Cooldown
        DWORD now = GetTickCount();
        if (now - s_lastWrite < (DWORD)Config::magicWriteCooldownMs) {
            return false;
        }

        // Kamera ve hedef
        FMinimalViewInfo pov;
        if (!Game::ReadCamera(pov)) return false;

        auto chars = Actors::GetCharacters();
        if (chars.empty()) return false;

        FVector target;
        uintptr_t targetActor;
        if (!FindTarget(chars, pov, target, targetActor)) return false;

        // Yeni spawn olan projectile'ı bul
        uintptr_t level = Memory::Read<uintptr_t>(Game::GWorld + 0x30);
        if (!level) return false;

        uintptr_t data = Memory::Read<uintptr_t>(level + 0x98);
        int32_t count = Memory::Read<int32_t>(level + 0xA0);
        if (!data || count <= 0 || count > 100000) return false;

        // Player pozisyonu
        FVector playerPos = pov.Location;

        // En yakın projectile'ı bul
        uintptr_t bestProjectile = 0;
        float bestDist = 500.0f;

        for (int i = 0; i < count; i++) {
            uintptr_t actor = Memory::Read<uintptr_t>(data + (uintptr_t)i * 8);
            if (!actor || actor < 0x10000) continue;

            uintptr_t cp = Memory::Read<uintptr_t>(actor + 0x10);
            if (!cp) continue;

            int32_t classIdx = Memory::Read<int32_t>(cp + 0x18);
            std::string className = UE4::ReadFName(Game::GNames, classIdx);

            if (className.find("Projectile") == std::string::npos) continue;

            uintptr_t root = Memory::Read<uintptr_t>(actor + 0x130);
            if (!root) continue;

            FVector projPos = Memory::Read<FVector>(root + 0x11C);
            float dist = (projPos - playerPos).Length();
            if (dist < bestDist) {
                bestDist = dist;
                bestProjectile = actor;
            }
        }

        if (!bestProjectile) return false;

        FVector projPos = Memory::Read<FVector>(
            Memory::Read<uintptr_t>(bestProjectile + 0x130) + 0x11C);

        // ProjectileMovementComponent ve Velocity offsetleri
        const uintptr_t PMC_OFFSET = 0x908;
        const uintptr_t VELOCITY_OFFSET = 0xc4;

        uintptr_t pmc = Memory::Read<uintptr_t>(bestProjectile + PMC_OFFSET);
        if (!pmc) return false;

        // Hedefe doğru velocity hesapla (yön değişir, hız korunur)
        FVector dir = target - projPos;
        dir = dir * (1.0f / dir.Length());

        FVector curVel = Memory::Read<FVector>(pmc + VELOCITY_OFFSET);
        float speed = curVel.Length();
        if (speed < 1.0f) speed = 3000.0f;

        FVector newVel = dir * speed;

        bool ok = WriteFVector(pmc + VELOCITY_OFFSET, newVel);

        if (ok) {
            s_lastWrite = now;
        }

        return ok;
    }
}
