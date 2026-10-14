#include "ESP_Characters.h"
#include "Draw.h"
#include "Config.h"
#include "Offsets.h"
#include "Memory.h"
#include "UE4.h"
#include <cstdio>
#include <cmath>
#include <cstring>

namespace ESP {

    void DrawCharacters(ImDrawList* dl,
        const std::vector<Actors::Character>& list,
        const FMinimalViewInfo& pov, int sw, int sh,
        const FVector& localPos, bool hasLocalPos) {

        for (const auto& ch : list) {
            if (!ch.rootComponent) continue;

            // ★ Takım filtresi
            if (!Config::espTeam && !ch.isEnemy && !ch.isLocalPlayer)
                continue;

            // ★ Ölü filtresi
            if (!Config::espDead && ch.isDead) continue;

            FVector world = Memory::Read<FVector>(
                ch.rootComponent + Offsets::Scene_RelativeLocation);

            // ★ Mesafe filtresi
            if (hasLocalPos && !ch.isLocalPlayer) {
                FVector d = world - localPos;
                float dist = d.Length() / 100.0f;
                if (dist > Config::espMaxDistance) continue;
            }

            FVector headWorld = world;  headWorld.Z += 90.0f;
            FVector feetWorld = world;  feetWorld.Z -= 90.0f;

            ImVec2 headScreen, feetScreen;
            if (!UE4::WorldToScreen(headWorld, headScreen, pov, sw, sh)) continue;
            if (!UE4::WorldToScreen(feetWorld, feetScreen, pov, sw, sh)) continue;

            if (headScreen.x < 0 || headScreen.x > sw ||
                headScreen.y < 0 || headScreen.y > sh) continue;

            float boxH = feetScreen.y - headScreen.y;
            if (boxH < 5.0f)    boxH = 5.0f;
            if (boxH > 1000.0f) boxH = 1000.0f;

            float boxW = boxH * 0.4f;

            ImVec2 tl(headScreen.x - boxW / 2, headScreen.y);
            ImVec2 br(headScreen.x + boxW / 2, feetScreen.y);

            ImU32 color;
            if (ch.isLocalPlayer)
                color = IM_COL32(255, 255, 0, 255);
            else if (ch.isEnemy)
                color = IM_COL32(255, 50, 50, 255);
            else
                color = IM_COL32(50, 255, 50, 255);

            // ★ Kutu
            if (Config::espBox) {
                Draw::Box(dl, tl, br, color, Config::espBoxThickness);
            }

            // ★ İsim
            if (Config::espName && ch.name[0] != '\0' &&
                strcmp(ch.name, "Unknown") != 0) {
                ImVec2 nameSize = ImGui::CalcTextSize(ch.name);
                ImVec2 namePos(tl.x + boxW / 2 - nameSize.x / 2, tl.y - 24);
                Draw::TextBgBold(dl, namePos, color, ch.name);
            }

            // ★ Mesafe
            if (Config::espDistance && hasLocalPos && !ch.isLocalPlayer) {
                FVector d = world - localPos;
                float dist = d.Length() / 100.0f;

                // Metin boyutunu al (ortalama için)
                char buf[16] = {};
                snprintf(buf, sizeof(buf), "%.0f m", dist);
                ImVec2 distSize = ImGui::CalcTextSize(buf);

                ImVec2 distPos(tl.x + boxW / 2 - distSize.x / 2, br.y + 4);
                Draw::DistanceLabel(dl, distPos, dist);
            }

            // ★ Sağlık
            if (Config::espHealth && ch.maxHealth > 0.0f) {
                float r = ch.health / ch.maxHealth;
                if (r < 0.0f) r = 0.0f;
                if (r > 1.0f) r = 1.0f;
                Draw::HealthBar(dl, ImVec2(tl.x, br.y + 26),
                    boxW, 4.0f, r);
            }
        }
    }
}