#pragma once
#include <d3d11.h>
#include <string>
#include <unordered_map>

namespace Assets {
    void Init(ID3D11Device* device, const std::string& iconFolder);
    void Shutdown();
    ID3D11ShaderResourceView* GetIcon(const std::string& itemName);
}