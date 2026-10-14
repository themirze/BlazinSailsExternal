#pragma once

namespace Menu {
    // Call once after ImGui::CreateContext().
    void Init();

    // Call every frame between ImGui::NewFrame() and ImGui::Render().
    void Render();
}
