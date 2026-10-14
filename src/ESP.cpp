#include "ESP.h"
#include "Game.h"
#include "Config.h"
#include "UE4.h"
#include "Overlay.h"
#include "Memory.h"
#include "Actors.h"
#include "ESP_Characters.h"
#include "ESP_Items.h"
#include "Aimbot.h"
#include "Offsets.h"
#include "Triggerbot.h"
#include "MagicBullet.h"
#include <Windows.h>
#include <imgui.h>

namespace ESP {

    // ============================================================
    // Oyun penceresini bul (cache'li)
    // ============================================================
    static HWND FindGameWindow() {
        // İlk: isimle bul
        HWND hwnd = FindWindowW(nullptr, L"Blazing Sails");
        if (hwnd) return hwnd;

        // Fallback: process ID ile bul
        if (!Game::processId) return nullptr;

        struct Data {
            DWORD pid;
            HWND  hwnd;
        };
        Data data = { Game::processId, nullptr };

        EnumWindows([](HWND h, LPARAM lp) -> BOOL {
            Data* d = (Data*)lp;
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid == d->pid && IsWindowVisible(h)) {
                // Ana pencere mi? (owner'ı yoksa ana penceredir)
                if (GetWindow(h, GW_OWNER) == nullptr) {
                    d->hwnd = h;
                    return FALSE;
                }
            }
            return TRUE;
            }, (LPARAM)&data);

        return data.hwnd;
    }

    // ============================================================
    // Ana render
    // ============================================================
    void Render() {
        if (!Game::IsAttached()) return;
        if (!Game::GWorld) return;

        // ===== Aimbot + Triggerbot + Magic Bullet güncelle =====
        Aimbot::Update();
        Triggerbot::Update();
        MagicBullet::Update();

        // ===== Overlay görünürlük kontrolü =====
        static HWND s_gameWindow = nullptr;
        static DWORD s_lastFind = 0;
        DWORD now = GetTickCount();

        // 5 saniyede bir pencereyi yeniden bul
        if (!s_gameWindow || now - s_lastFind > 5000) {
            s_gameWindow = FindGameWindow();
            s_lastFind = now;
        }

        HWND foreground = GetForegroundWindow();
        bool gameOnTop = (s_gameWindow && foreground == s_gameWindow);
        bool menuOpen = Config::menuOpen;

        bool shouldShow = gameOnTop || menuOpen;

        static bool s_overlayVisible = true;
        if (shouldShow != s_overlayVisible) {
            s_overlayVisible = shouldShow;
            ShowWindow(Overlay::hwnd, shouldShow ? SW_SHOW : SW_HIDE);
        }

        // Oyun ön planda değilse ve menü kapalıysa hiç çizme
        if (!shouldShow) return;

        // ===== ESP görünürlük kontrolü =====
        if (!Config::espEnabled) return;

        FMinimalViewInfo pov;
        if (!Game::ReadCamera(pov)) return;

        int sw = (int)ImGui::GetIO().DisplaySize.x;
        int sh = (int)ImGui::GetIO().DisplaySize.y;
        if (sw <= 0 || sh <= 0) return;

        ImDrawList* dl = ImGui::GetBackgroundDrawList();

        FVector localPos(0, 0, 0);
        bool hasLocalPos = false;
        if (Game::localPawn) {
            uintptr_t root = Memory::Read<uintptr_t>(
                Game::localPawn + Offsets::Actor_RootComponent);
            if (root) {
                localPos = Memory::Read<FVector>(
                    root + Offsets::Scene_RelativeLocation);
                hasLocalPos = true;
            }
        }

        if (Config::espCharacters) {
            auto chars = Actors::GetCharacters();
            DrawCharacters(dl, chars, pov, sw, sh, localPos, hasLocalPos);
        }

        if (Config::espItems) {
            auto items = Actors::GetItems();
            DrawItems(dl, items, pov, sw, sh, localPos, hasLocalPos);
        }

        // FOV dairesi
        Aimbot::RenderFOV(dl, sw, sh);
    }
}
