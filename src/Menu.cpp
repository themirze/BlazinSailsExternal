#include "Menu.h"
#include "Theme.h"
#include "Config.h"
#include <imgui.h>

namespace Menu {

    namespace {
        int   tab = 0;
        float uiScale = 1.f;

        bool TabButton(const char* label, bool selected) {
            if (selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, Theme::Accent);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.05f, 0.05f, 0.07f, 1.f));
            }
            bool clicked = ImGui::Button(label, ImVec2(-FLT_MIN, 36));
            if (selected) ImGui::PopStyleColor(2);
            return clicked;
        }

        // Ayrı fonksiyonlar — her biri tek iş yapar
        void SliderF(const char* id, float* v, float lo, float hi, const char* fmt) {
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SliderFloat(id, v, lo, hi, fmt);
        }

        void SliderI(const char* id, int* v, int lo, int hi, const char* fmt) {
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SliderInt(id, v, lo, hi, fmt);
        }

        void CharacterTab() {
            Theme::SectionTitle("Characters");
            Theme::Toggle("Enable ESP", &Config::espEnabled);
            Theme::Toggle("Characters", &Config::espCharacters);
            Theme::Toggle("Box", &Config::espBox);
            Theme::Toggle("Name", &Config::espName);
            Theme::Toggle("Distance", &Config::espDistance);
            Theme::Toggle("Health", &Config::espHealth);
            Theme::Toggle("Show team", &Config::espTeam);
            Theme::Toggle("Show dead", &Config::espDead);
            SliderF("##maxdist", &Config::espMaxDistance, 50.f, 2000.f, "Max distance: %.0f m");

            ImGui::Spacing();
            Theme::SectionTitle("Colors");
            SliderF("##thick", &Config::espBoxThickness, 0.5f, 5.f, "Box thickness: %.1f");
            const ImGuiColorEditFlags f = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar;
            ImGui::ColorEdit4("Enemy", Config::colorEnemy, f);
            ImGui::ColorEdit4("Team", Config::colorTeam, f);
            ImGui::ColorEdit4("Text", Config::colorText, f);
        }

        void ItemTab() {
            Theme::SectionTitle("Item ESP");
            Theme::Toggle("Enable item ESP", &Config::espItems);
            SliderF("##itemdist", &Config::espItemMaxDistance, 50.f, 1500.f, "Max distance: %.0f m");

            ImGui::Spacing();
            Theme::SectionTitle("Item list");
            Theme::Toggle("Chests", &Config::itemShowChests);
            Theme::Toggle("Weapons", &Config::itemShowWeapons);
            Theme::Toggle("Ammo", &Config::itemShowAmmo);
            Theme::Toggle("Barrels", &Config::itemShowBarrels);
            Theme::Toggle("Upgrades", &Config::itemShowUpgrades);
            Theme::Toggle("Cannons", &Config::itemShowCannons);
            Theme::Toggle("Resources", &Config::itemShowResources);
            Theme::Toggle("Paddles", &Config::itemShowPaddles);
            Theme::Toggle("Other", &Config::itemShowOther);
        }

        // ============================================
        // AIM — sadece Aimlock + Magic Bullet (input)
        // ============================================
        void AimTab() {
            Theme::SectionTitle("Aimlock");
            Theme::Toggle("Enable aimlock", &Config::aimlockEnabled);

            ImGui::TextDisabled("Trigger keys:");
            Theme::Toggle("Right mouse##alm", &Config::aimlockOnRightMouse);
            Theme::Toggle("Left mouse##alm", &Config::aimlockOnLeftMouse);
            Theme::Toggle("CTRL##alm", &Config::aimlockOnCtrl);
            Theme::Toggle("SHIFT##alm", &Config::aimlockOnShift);
            Theme::Toggle("ALT##alm", &Config::aimlockOnAlt);

            SliderF("##smooth", &Config::aimlockSmooth, 1.f, 20.f, "Smoothness: %.1f");
            SliderF("##maxstep", &Config::aimlockMaxStep, 1.f, 50.f, "Max step: %.1f px/frame");

            ImGui::Spacing();
            Theme::SectionTitle("Magic Bullet (Input)");
            Theme::Toggle("Enable magic bullet", &Config::magicEnabled);
            Theme::Toggle("Magic on left mouse", &Config::magicOnLeftMouse);
            Theme::Toggle("Magic on right mouse", &Config::magicOnRightMouse);
            SliderF("##magicsmooth", &Config::magicSmooth, 0.1f, 5.f, "Smoothness: %.1f");

            ImGui::Spacing();
            Theme::SectionTitle("Common");
            Theme::Toggle("Draw FOV circle", &Config::aimDrawFOV);
            SliderF("##fov", &Config::aimFOV, 30.f, 360.f, "FOV: %.0f");
            Theme::Toggle("Aim at team", &Config::aimTeam);

            ImGui::Spacing();
            Theme::SectionTitle("Bone Target");
            const char* bones[] = { "Head", "Chest", "Feet" };
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Combo("##bone", &Config::aimBone, bones, 3);
            SliderF("##headoff", &Config::aimHeadOffset, -100.f, 100.f, "Head offset: %.1f");
            SliderF("##chestoff", &Config::aimChestOffset, -100.f, 100.f, "Chest offset: %.1f");
            SliderF("##footoff", &Config::aimFootOffset, -150.f, 50.f, "Feet offset: %.1f");
        }

        // ============================================
        // MISC — Triggerbot + ileride başka özellikler
        // ============================================
        void MiscTab() {
            Theme::SectionTitle("Triggerbot");
            ImGui::TextWrapped("Auto-fire when crosshair is on enemy while holding a trigger key.");
            ImGui::Spacing();

            Theme::Toggle("Enable triggerbot", &Config::triggerEnabled);

            ImGui::TextDisabled("Trigger keys:");
            Theme::Toggle("Right mouse##trg", &Config::triggerOnRightMouse);
            Theme::Toggle("CTRL##trg", &Config::triggerOnCtrl);
            Theme::Toggle("SHIFT##trg", &Config::triggerOnShift);

            Theme::Toggle("Head only", &Config::triggerHeadOnly);

            SliderI("##trigdelay", &Config::triggerDelayMs, 0, 300, "Delay: %d ms (0 = instant)");
            SliderF("##trigfov", &Config::triggerFOV, 1.f, 50.f, "FOV radius: %.1f px");

            ImGui::Spacing();
            Theme::SectionTitle("Info");
            ImGui::TextWrapped("Hold RIGHT MOUSE (ADS) + crosshair on enemy → auto fires.");
            ImGui::TextDisabled("More features coming soon...");
        }

        void SettingsTab() {
            Theme::SectionTitle("Interface");
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::SliderFloat("##scale", &uiScale, 0.8f, 1.6f, "UI scale: %.2f"))
                ImGui::GetIO().FontGlobalScale = uiScale;
            ImGui::TextDisabled("Toggle menu with INSERT");
        }
    }

    void Init() {
        Theme::Apply();
    }

    void Render() {
        if (!Config::menuOpen) return;

        ImGui::SetNextWindowSize(ImVec2(650, 550), ImGuiCond_FirstUseEver);
        ImGui::Begin("##BlazingMenu", &Config::menuOpen,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

        ImGui::BeginChild("##sidebar", ImVec2(140, 0), ImGuiChildFlags_Borders);
        if (TabButton("Characters", tab == 0)) tab = 0;
        if (TabButton("Items", tab == 1)) tab = 1;
        if (TabButton("Aim", tab == 2)) tab = 2;
        if (TabButton("Misc", tab == 3)) tab = 3;
        if (TabButton("Settings", tab == 4)) tab = 4;
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##content", ImVec2(0, 0), ImGuiChildFlags_Borders);
        switch (tab) {
        case 0: CharacterTab(); break;
        case 1: ItemTab();      break;
        case 2: AimTab();       break;
        case 3: MiscTab();      break;
        default: SettingsTab(); break;
        }
        ImGui::EndChild();

        ImGui::End();
    }
}