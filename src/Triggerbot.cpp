#include "Triggerbot.h"
#include "Config.h"
#include "Game.h"
#include "Memory.h"
#include "Offsets.h"
#include "UE4.h"
#include "Actors.h"
#include <Windows.h>
#include <cmath>

namespace Triggerbot {

    static DWORD s_lastShot = 0;
    static bool  s_mouseDown = false;
    static DWORD s_mouseDownAt = 0;

    // Basılı tut → ateş et, 40ms sonra bırak (oyun input okuması için)
    static void FireOnce() {
        if (s_mouseDown) return;

        INPUT down = {};
        down.type = INPUT_MOUSE;
        down.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &down, sizeof(INPUT));

        s_mouseDown = true;
        s_mouseDownAt = GetTickCount();
    }

    static void UpdateMouseRelease() {
        if (!s_mouseDown) return;
        DWORD now = GetTickCount();
        if (now - s_mouseDownAt > 40) {
            INPUT up = {};
            up.type = INPUT_MOUSE;
            up.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &up, sizeof(INPUT));
            s_mouseDown = false;
        }
    }

    // Çoklu tetik kontrolü (sol tık hariç)
    static bool IsActivationKeyDown() {
        if (Config::triggerOnRightMouse && (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) return true;
        if (Config::triggerOnCtrl && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) return true;
        if (Config::triggerOnShift && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) return true;
        return false;
    }

    static FVector GetTargetPos(const Actors::Character& ch, int bone) {
        FVector pos = Memory::Read<FVector>(
            ch.rootComponent + Offsets::Scene_RelativeLocation);

        uintptr_t mesh = Memory::Read<uintptr_t>(
            ch.actor + Offsets::Char_Mesh);
        if (mesh) {
            FVector mp = Memory::Read<FVector>(mesh + 0x100);
            if (mp.X != 0.0f && mp.Y != 0.0f && fabsf(mp.X) < 10000000.0f)
                pos = mp;
        }

        switch (bone) {
        case 0: pos.Z += Config::aimHeadOffset; break;
        case 1: pos.Z += Config::aimChestOffset; break;
        case 2: pos.Z += Config::aimFootOffset; break;
        }
        return pos;
    }

    void Update() {
        // Önce bekleyen mouse-up işlemini yap
        UpdateMouseRelease();

        if (!Config::triggerEnabled) return;
        if (!Game::IsAttached() || !Game::GWorld) return;

        // Tetik tuşu basılı değilse çık
        if (!IsActivationKeyDown()) return;

        // Cooldown (0 = delay yok)
        DWORD now = GetTickCount();
        if (Config::triggerDelayMs > 0) {
            if (now - s_lastShot < (DWORD)Config::triggerDelayMs) return;
        }

        FMinimalViewInfo pov;
        if (!Game::ReadCamera(pov)) return;

        int sw = (int)ImGui::GetIO().DisplaySize.x;
        int sh = (int)ImGui::GetIO().DisplaySize.y;
        if (sw <= 0 || sh <= 0) return;

        float cx = sw * 0.5f;
        float cy = sh * 0.5f;

        auto chars = Actors::GetCharacters();
        if (chars.empty()) return;

        for (const auto& ch : chars) {
            if (!ch.rootComponent) continue;
            if (ch.isDead || ch.isLocalPlayer) continue;
            if (!Config::aimTeam && !ch.isEnemy) continue;

            FVector world = GetTargetPos(ch, Config::triggerHeadOnly ? 0 : 1);

            ImVec2 sp;
            if (!UE4::WorldToScreen(world, sp, pov, sw, sh)) continue;

            float dx = sp.x - cx;
            float dy = sp.y - cy;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist < Config::triggerFOV) {
                FireOnce();
                s_lastShot = now;
                return;
            }
        }
    }
}