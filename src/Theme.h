#pragma once
#include <imgui.h>
#include <algorithm>

namespace Theme {

    // Accent color used across the UI (warm "blazing" orange).
    inline constexpr ImVec4 Accent      = ImVec4(1.00f, 0.45f, 0.15f, 1.00f);
    inline constexpr ImVec4 AccentHover = ImVec4(1.00f, 0.55f, 0.28f, 1.00f);
    inline constexpr ImVec4 AccentDim   = ImVec4(1.00f, 0.45f, 0.15f, 0.35f);

    inline void Apply() {
        ImGuiStyle& s = ImGui::GetStyle();

        // Geometry
        s.WindowPadding     = ImVec2(16, 16);
        s.FramePadding      = ImVec2(10, 6);
        s.ItemSpacing       = ImVec2(10, 10);
        s.ItemInnerSpacing  = ImVec2(8, 6);
        s.ScrollbarSize     = 10.0f;
        s.GrabMinSize       = 12.0f;

        s.WindowRounding    = 10.0f;
        s.ChildRounding     = 8.0f;
        s.FrameRounding     = 6.0f;
        s.PopupRounding     = 8.0f;
        s.ScrollbarRounding = 10.0f;
        s.GrabRounding      = 6.0f;
        s.TabRounding       = 6.0f;

        s.WindowBorderSize  = 1.0f;
        s.ChildBorderSize   = 1.0f;
        s.FrameBorderSize   = 0.0f;
        s.PopupBorderSize   = 1.0f;
        s.WindowTitleAlign  = ImVec2(0.5f, 0.5f);

        // Colors
        ImVec4* c = s.Colors;
        const ImVec4 bg      = ImVec4(0.07f, 0.07f, 0.09f, 0.96f);
        const ImVec4 panel   = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
        const ImVec4 widget  = ImVec4(0.15f, 0.15f, 0.19f, 1.00f);
        const ImVec4 widgetH = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
        const ImVec4 border  = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);

        c[ImGuiCol_Text]                 = ImVec4(0.92f, 0.93f, 0.96f, 1.00f);
        c[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.52f, 0.58f, 1.00f);
        c[ImGuiCol_WindowBg]             = bg;
        c[ImGuiCol_ChildBg]              = panel;
        c[ImGuiCol_PopupBg]              = panel;
        c[ImGuiCol_Border]               = border;
        c[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_FrameBg]              = widget;
        c[ImGuiCol_FrameBgHovered]       = widgetH;
        c[ImGuiCol_FrameBgActive]        = widgetH;
        c[ImGuiCol_TitleBg]              = bg;
        c[ImGuiCol_TitleBgActive]        = bg;
        c[ImGuiCol_TitleBgCollapsed]     = bg;
        c[ImGuiCol_ScrollbarBg]          = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_ScrollbarGrab]        = widgetH;
        c[ImGuiCol_ScrollbarGrabHovered] = AccentDim;
        c[ImGuiCol_ScrollbarGrabActive]  = Accent;
        c[ImGuiCol_CheckMark]            = Accent;
        c[ImGuiCol_SliderGrab]           = Accent;
        c[ImGuiCol_SliderGrabActive]     = AccentHover;
        c[ImGuiCol_Button]               = widget;
        c[ImGuiCol_ButtonHovered]        = AccentDim;
        c[ImGuiCol_ButtonActive]         = Accent;
        c[ImGuiCol_Header]               = AccentDim;
        c[ImGuiCol_HeaderHovered]        = ImVec4(1.00f, 0.45f, 0.15f, 0.50f);
        c[ImGuiCol_HeaderActive]         = Accent;
        c[ImGuiCol_Separator]            = border;
        c[ImGuiCol_Tab]                  = widget;
        c[ImGuiCol_TabHovered]           = AccentHover;
        c[ImGuiCol_TabSelected]          = Accent;
    }

    // Small helper: section label in accent color with a divider underneath.
    inline void SectionTitle(const char* label) {
        ImGui::PushStyleColor(ImGuiCol_Text, Accent);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::Separator();
    }

    // Simple checkbox replacement with accent color
    inline void Toggle(const char* label, bool* v) {
        ImGui::PushID(v);          // ★ pointer'ı ID olarak kullan — unique
        ImGui::Checkbox(label, v);
        ImGui::PopID();
    }
}
