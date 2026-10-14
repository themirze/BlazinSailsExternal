#pragma once
#include <imgui.h>

namespace Aimbot {
    void Update();                          // Her frame çağrılır
    void RenderFOV(ImDrawList* dl, int sw, int sh);  // FOV dairesi
}