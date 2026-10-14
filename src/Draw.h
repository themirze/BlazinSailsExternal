#pragma once
#include <imgui.h>
#include <cstdio>

namespace Draw {

    // Kutu çiz
    inline void Box(ImDrawList* dl, ImVec2 tl, ImVec2 br, ImU32 color,
        float thickness = 2.0f) {
        dl->AddRect(tl, br, color, 0.0f, 0, thickness);
    }

    // BOLD + arka planlı yazı
    inline void TextBgBold(ImDrawList* dl, ImVec2 pos, ImU32 color,
        const char* text,
        ImU32 bgColor = IM_COL32(0, 0, 0, 220)) {
        if (!text || text[0] == '\0') return;

        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImVec2 padding(6.0f, 3.0f);
        ImVec2 bgMin(pos.x - padding.x, pos.y - padding.y);
        ImVec2 bgMax(pos.x + textSize.x + padding.x,
            pos.y + textSize.y + padding.y);

        // Gölge
        dl->AddRectFilled(
            ImVec2(bgMin.x + 2, bgMin.y + 2),
            ImVec2(bgMax.x + 2, bgMax.y + 2),
            IM_COL32(0, 0, 0, 150), 5.0f);

        // Rounded arka plan
        dl->AddRectFilled(bgMin, bgMax, bgColor, 5.0f);

        // Kenarlık (item rengi ile)
        dl->AddRect(bgMin, bgMax, color, 5.0f, 0, 1.5f);

        // BOLD efekti: 4 yönde 0.7px offset + merkez
        const float b = 0.7f;
        dl->AddText(ImVec2(pos.x - b, pos.y), color, text);
        dl->AddText(ImVec2(pos.x + b, pos.y), color, text);
        dl->AddText(ImVec2(pos.x, pos.y - b), color, text);
        dl->AddText(ImVec2(pos.x, pos.y + b), color, text);
        dl->AddText(pos, color, text);
    }

    // Sağlık barı
    inline void HealthBar(ImDrawList* dl, ImVec2 tl, float width,
        float height, float ratio) {
        ImVec2 br(tl.x + width, tl.y + height);
        dl->AddRectFilled(tl, br, IM_COL32(0, 0, 0, 200));
        ImU32 color = IM_COL32(
            (int)(255 * (1.0f - ratio)),
            (int)(255 * ratio), 0, 255);
        ImVec2 fill(tl.x + width * ratio, br.y);
        dl->AddRectFilled(tl, fill, color);
        dl->AddRect(tl, br, IM_COL32(255, 255, 255, 200));
    }

    // Nokta
    inline void Dot(ImDrawList* dl, ImVec2 pos, ImU32 color, float r = 3.0f) {
        dl->AddCircleFilled(pos, r, color);
    }

    // Pill-style mesafe etiketi (sarı bg + siyah yazı)
    inline void DistanceLabel(ImDrawList* dl, ImVec2 pos, float meters) {
        char buf[32] = {};
        snprintf(buf, sizeof(buf), "%.0f m", meters);

        ImVec2 textSize = ImGui::CalcTextSize(buf);
        ImVec2 padding(7.0f, 3.0f);

        ImVec2 bgMin(pos.x - padding.x, pos.y - padding.y);
        ImVec2 bgMax(pos.x + textSize.x + padding.x,
            pos.y + textSize.y + padding.y);

        const float round = (bgMax.y - bgMin.y) * 0.5f;

        // Opak sarı arka plan + siyah yazı
        const ImU32 bgColor = IM_COL32(255, 220, 60, 255);
        const ImU32 textColor = IM_COL32(0, 0, 0, 255);

        // Gölge
        dl->AddRectFilled(
            ImVec2(bgMin.x + 1.0f, bgMin.y + 2.0f),
            ImVec2(bgMax.x + 1.0f, bgMax.y + 2.0f),
            IM_COL32(0, 0, 0, 100), round);

        // Ana pill arka plan
        dl->AddRectFilled(bgMin, bgMax, bgColor, round);

        // İnce koyu kenarlık
        dl->AddRect(bgMin, bgMax,
            IM_COL32(0, 0, 0, 120), round, 0, 1.0f);

        // BOLD + net yazı (4 yönde offset + merkez)
        const float b = 0.6f;
        dl->AddText(ImVec2(pos.x - b, pos.y), textColor, buf);
        dl->AddText(ImVec2(pos.x + b, pos.y), textColor, buf);
        dl->AddText(ImVec2(pos.x, pos.y - b), textColor, buf);
        dl->AddText(ImVec2(pos.x, pos.y + b), textColor, buf);
        dl->AddText(pos, textColor, buf);
    }
}
