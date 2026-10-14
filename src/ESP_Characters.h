#pragma once
#include <imgui.h>
#include "Actors.h"
#include "UE4.h"

namespace ESP {
    void DrawCharacters(ImDrawList* dl, const std::vector<Actors::Character>& list,
        const FMinimalViewInfo& pov, int sw, int sh,
        const FVector& localPos, bool hasLocalPos);
}