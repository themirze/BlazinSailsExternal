#include "Aimbot.h"
#include "Config.h"
#include "Game.h"
#include "Memory.h"
#include "Offsets.h"
#include "UE4.h"
#include "Actors.h"
#include <Windows.h>
#include <cmath>

namespace Aimbot {

    static uintptr_t s_lockedTarget = 0;

    // ============================================================
    // Fare relatif hareketi
    // ============================================================
    static void MoveMouse(float dx, float dy) {
        if (fabsf(dx) < 0.1f && fabsf(dy) < 0.1f) return;
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dx = (LONG)dx;
        input.mi.dy = (LONG)dy;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &input, sizeof(INPUT));
    }

    static bool IsRightMouseDown() {
        return (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    }
    static bool IsLeftMouseDown() {
        return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    }

    // ============================================================
    // ★ Aimlock tetik kontrolü (çoklu tuş)
// ============================================================
    static bool IsAimlockTriggerDown() {
        if (Config::aimlockOnRightMouse && (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) return true;
        if (Config::aimlockOnLeftMouse && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) return true;
        if (Config::aimlockOnCtrl && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) return true;
        if (Config::aimlockOnShift && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) return true;
        if (Config::aimlockOnAlt && (GetAsyncKeyState(VK_MENU) & 0x8000)) return true;
        return false;
    }

    // ============================================================
    // Hedef pozisyonu
    // ============================================================
    static FVector GetTargetPosition(const Actors::Character& ch) {
        FVector pos = Memory::Read<FVector>(
            ch.rootComponent + Offsets::Scene_RelativeLocation);

        uintptr_t mesh = Memory::Read<uintptr_t>(
            ch.actor + Offsets::Char_Mesh);
        if (mesh) {
            FVector meshPos = Memory::Read<FVector>(mesh + 0x100);
            if (meshPos.X != 0.0f && meshPos.Y != 0.0f &&
                fabsf(meshPos.X) < 10000000.0f) {
                pos = meshPos;
            }
        }

        switch (Config::aimBone) {
        case 0: pos.Z += Config::aimHeadOffset; break;
        case 1: pos.Z += Config::aimChestOffset; break;
        case 2: pos.Z += Config::aimFootOffset; break;
        }
        return pos;
    }

    // ============================================================
    // Hedef bul (kilit destekli)
    // ============================================================
    static bool FindTarget(
        const std::vector<Actors::Character>& chars,
        const FMinimalViewInfo& pov,
        int sw, int sh,
        ImVec2& outScreen)
    {
        float cx = sw * 0.5f;
        float cy = sh * 0.5f;

        // Kilitli hedefi takip et
        if (s_lockedTarget != 0) {
            for (const auto& ch : chars) {
                if (ch.actor != s_lockedTarget) continue;
                if (ch.isDead || ch.isLocalPlayer) {
                    s_lockedTarget = 0;
                    break;
                }
                FVector world = GetTargetPosition(ch);
                ImVec2 sp;
                if (UE4::WorldToScreen(world, sp, pov, sw, sh)) {
                    float dx = sp.x - cx;
                    float dy = sp.y - cy;
                    if (sqrtf(dx * dx + dy * dy) < Config::aimFOV * 1.8f) {
                        outScreen = sp;
                        return true;
                    }
                }
                s_lockedTarget = 0;
                break;
            }
        }

        // Yeni hedef
        float bestDist = Config::aimFOV;
        bool found = false;
        uintptr_t bestActor = 0;

        for (const auto& ch : chars) {
            if (!ch.rootComponent) continue;
            if (ch.isDead) continue;
            if (ch.isLocalPlayer) continue;
            if (!Config::aimTeam && !ch.isEnemy) continue;

            FVector world = GetTargetPosition(ch);
            ImVec2 sp;
            if (!UE4::WorldToScreen(world, sp, pov, sw, sh)) continue;

            float dx = sp.x - cx;
            float dy = sp.y - cy;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist < bestDist) {
                bestDist = dist;
                outScreen = sp;
                bestActor = ch.actor;
                found = true;
            }
        }

        if (found) s_lockedTarget = bestActor;
        return found;
    }

    // ============================================================
    // AIMLOCK — yumuşak takip
    // ============================================================
    static void DoAimlock() {
        if (!Config::aimlockEnabled) return;

        // ★ Çoklu tetik kontrolü
        if (!IsAimlockTriggerDown()) {
            s_lockedTarget = 0;
            return;
        }

        FMinimalViewInfo pov;
        if (!Game::ReadCamera(pov)) return;

        int sw = (int)ImGui::GetIO().DisplaySize.x;
        int sh = (int)ImGui::GetIO().DisplaySize.y;
        if (sw <= 0 || sh <= 0) return;

        auto chars = Actors::GetCharacters();
        if (chars.empty()) return;

        ImVec2 target;
        if (!FindTarget(chars, pov, sw, sh, target)) return;

        float cx = sw * 0.5f;
        float cy = sh * 0.5f;
        float dx = target.x - cx;
        float dy = target.y - cy;

        const float DEADZONE = 2.0f;
        if (fabsf(dx) < DEADZONE && fabsf(dy) < DEADZONE) return;

        float smooth = Config::aimlockSmooth;
        if (smooth < 1.0f) smooth = 1.0f;
        dx /= smooth;
        dy /= smooth;

        float maxStep = Config::aimlockMaxStep;
        if (maxStep < 1.0f) maxStep = 1.0f;
        if (dx > maxStep) dx = maxStep;
        if (dx < -maxStep) dx = -maxStep;
        if (dy > maxStep) dy = maxStep;
        if (dy < -maxStep) dy = -maxStep;

        MoveMouse(dx, dy);
    }

    // ============================================================
    // MAGIC BULLET — her frame hedefe snap
    // ============================================================
    static void DoMagicBullet() {
        if (!Config::magicEnabled) return;

        bool fireDown = false;
        if (Config::magicOnLeftMouse && IsLeftMouseDown())  fireDown = true;
        if (Config::magicOnRightMouse && IsRightMouseDown()) fireDown = true;

        if (!fireDown) return;

        FMinimalViewInfo pov;
        if (!Game::ReadCamera(pov)) return;

        int sw = (int)ImGui::GetIO().DisplaySize.x;
        int sh = (int)ImGui::GetIO().DisplaySize.y;
        if (sw <= 0 || sh <= 0) return;

        auto chars = Actors::GetCharacters();
        if (chars.empty()) return;

        ImVec2 target;
        if (!FindTarget(chars, pov, sw, sh, target)) return;

        float cx = sw * 0.5f;
        float cy = sh * 0.5f;
        float dx = target.x - cx;
        float dy = target.y - cy;

        if (fabsf(dx) < 1.0f && fabsf(dy) < 1.0f) return;

        // Her frame anlık snap
        float smooth = Config::magicSmooth;
        if (smooth < 1.0f) smooth = 1.0f;
        dx /= smooth;
        dy /= smooth;

        MoveMouse(dx, dy);
    }

    // ============================================================
    // Ana güncelleme
    // ============================================================
    void Update() {
        if (!Game::IsAttached()) return;
        if (!Game::GWorld) return;

        // Magic Bullet öncelikli
        if (Config::magicEnabled) {
            DoMagicBullet();
            return;
        }

        // Aimlock
        if (Config::aimlockEnabled) {
            DoAimlock();
        }

    }

    // ============================================================
    // FOV dairesi
    // ============================================================
    void RenderFOV(ImDrawList* dl, int sw, int sh) {
        if (!Config::aimlockEnabled && !Config::magicEnabled) return;
        if (!Config::aimDrawFOV) return;

        float cx = sw * 0.5f;
        float cy = sh * 0.5f;
        float r = Config::aimFOV;

        ImU32 color = (s_lockedTarget != 0)
            ? IM_COL32(255, 50, 50, 220)
            : IM_COL32(255, 255, 255, 180);

        dl->AddCircle(ImVec2(cx, cy), r, color, 64, 1.5f);
        dl->AddCircle(ImVec2(cx, cy), r * 0.5f,
            IM_COL32(255, 255, 255, 60), 64, 1.0f);
        dl->AddCircleFilled(ImVec2(cx, cy), 2.0f, IM_COL32(255, 0, 0, 200));
    }
}