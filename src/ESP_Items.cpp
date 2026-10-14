#include "ESP_Items.h"
#include "Draw.h"
#include "Config.h"
#include "ItemClasses.h"
#include "Assets.h"
#include <cstdio>
#include <cmath>

namespace ESP {

    static bool IsCategoryEnabled(ItemClasses::Category cat) {
        using C = ItemClasses::Category;
        switch (cat) {
        case C::Chest:    return Config::itemShowChests;
        case C::Weapon:   return Config::itemShowWeapons;
        case C::Ammo:     return Config::itemShowAmmo;
        case C::Barrel:   return Config::itemShowBarrels;
        case C::Upgrade:  return Config::itemShowUpgrades;
        case C::Cannon:   return Config::itemShowCannons;
        case C::Resource: return Config::itemShowResources;
        case C::Paddle:   return Config::itemShowPaddles;
        case C::Other:    return Config::itemShowOther;
        }
        return true;
    }

    void DrawItems(ImDrawList* dl,
        const std::vector<Actors::Item>& list,
        const FMinimalViewInfo& pov, int sw, int sh,
        const FVector& localPos, bool hasLocalPos) {

        for (const auto& it : list) {
            // Kategori filtresi
            ItemClasses::Category cat = ItemClasses::GetCategory(it.className);
            if (!IsCategoryEnabled(cat)) continue;

            ImVec2 sp;
            if (!UE4::WorldToScreen(it.worldPos, sp, pov, sw, sh)) continue;
            if (sp.x < 0 || sp.x > sw || sp.y < 0 || sp.y > sh) continue;

            float dist = 0.0f;
            if (hasLocalPos) {
                FVector d = it.worldPos - localPos;
                dist = d.Length() / 100.0f;
                if (dist > Config::espItemMaxDistance) continue;
            }

            ImU32 color;
            const char* icon = ItemClasses::GetIcon(it.className);
            std::string ic(icon);

            if (ic == "[CANNON]")       color = IM_COL32(255, 100, 100, 255);
            else if (ic == "[CHEST]")   color = IM_COL32(255, 215, 0, 255);
            else if (ic == "[LOOT]")    color = IM_COL32(255, 215, 0, 255);
            else if (ic == "[AMMO]")    color = IM_COL32(255, 255, 100, 255);
            else if (ic == "[BARREL]")  color = IM_COL32(255, 150, 50, 255);
            else if (ic == "[UPGRADE]") color = IM_COL32(150, 100, 255, 255);
            else if (ic == "[WOOD]")    color = IM_COL32(180, 130, 80, 255);
            else if (ic == "[PADDLE]")  color = IM_COL32(160, 160, 160, 255);
            else                        color = IM_COL32(100, 200, 255, 255);

            ID3D11ShaderResourceView* srv = Assets::GetIcon(it.className);
            const float ICON_SIZE = 28.0f;

            if (srv) {
                // ★ PNG ikonu çiz
                ImVec2 iconMin(sp.x - ICON_SIZE / 2.0f,
                    sp.y - ICON_SIZE / 2.0f);
                ImVec2 iconMax(sp.x + ICON_SIZE / 2.0f,
                    sp.y + ICON_SIZE / 2.0f);
                dl->AddImage((ImTextureID)(intptr_t)srv, iconMin, iconMax);

                // ★ Pill-style mesafe etiketi
                if (hasLocalPos) {
                    Draw::DistanceLabel(dl,
                        ImVec2(sp.x + ICON_SIZE / 2.0f + 6.0f,
                            sp.y - 8.0f),
                        dist);
                }
            }
            else {
                // ★ İkon yoksa: nokta + text
                Draw::Dot(dl, sp, color, 3.0f);

                char buf[128] = {};
                if (hasLocalPos)
                    snprintf(buf, sizeof(buf), "%s %s [%.0fm]",
                        icon, it.className.c_str(), dist);
                else
                    snprintf(buf, sizeof(buf), "%s %s",
                        icon, it.className.c_str());

                Draw::TextBgBold(dl, ImVec2(sp.x + 8, sp.y - 10),
                    color, buf);
            }
        }
    }
}