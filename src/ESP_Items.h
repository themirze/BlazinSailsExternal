#pragma once
#include <imgui.h>
#include "Actors.h"
#include "UE4.h"

namespace ESP {
    void DrawItems(ImDrawList* dl, const std::vector<Actors::Item>& list,
        const FMinimalViewInfo& pov, int sw, int sh,
        const FVector& localPos, bool hasLocalPos);
}