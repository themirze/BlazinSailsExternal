#include "Assets.h"
#include <wincodec.h>
#include <unordered_map>
#include <string>
#include <vector>

#pragma comment(lib, "windowscodecs.lib")

namespace Assets {
    static std::unordered_map<std::string, ID3D11ShaderResourceView*> g_icons;
    static const std::unordered_map<std::string, std::string> ICON_MAP = {
        // ===== Silahlar =====
        { "Flintlock",          "IC_flintlock" },
        { "Blunderbuss",        "IC_blunderbuss" },
        { "Crossbow",           "IC_Crossbow" },
        { "Musket",             "IC_musket" },
        { "Club",               "IC_Club" },
        { "Repeater",           "IC_Repeater" },
        { "DoubleBarrel",       "IC_doublebarrel" },
        { "HealingCrossbow",    "HealingCrossbow" },
        { "Cutlass",            "IC_cutlass" },
        { "Sniper",             "IC_fastsniper" },
        { "Pickaxe",            "IC_Pickaxe" },
        { "Shovel",             "IC_Shovel" },
        { "ButchersKnife",      "IC_butchersknife" },
        { "DualFlintlock",      "IC_doubleflintlock" },
        { "QuadFlintlock",      "IC_quadflintlock" },
        { "DualQuadFlintlock",  "IC_doublequadflintlock" },
        { "DragonPistol",       "IC_dragon_pistol" },
        { "HalfBarrel",         "IC_doublebarrel" },
        { "BombLauncher",       "IC_bomblauncher" },
        { "BrokenBottle",       "IC_bottle" },

        // ===== Sandıklar =====
        { "Lootcrate",          "lootcraft" },
        { "TreasureChest",      "Chest_Tier_1" },
        { "AmmoBox",            "Ammobag" },
        { "WoodCrate",          "NormalChest" },
        { "WoodBarrel",         "TntBarrel" },
        { "CarryChest",         "NormalChest" },
        { "ExplosiveBarrel",    "TntBarrel" },

        // ===== Mermiler =====
        { "Arrows",             "Arrows" },
        { "SmallShot",          "Ammobag" },
        { "HeavyShot",          "HeavyCannoball" },
        { "CannonBalls",        "Cannonball" },
        { "FishAmmo",           "FishAmmo" },
        { "AntiCannon",         "Cannonball_AntiCannon" },
        { "AntiSail",           "Cannonball_AntiSail" },

        // ===== Upgrade'ler =====
        { "CannonUpgrade",      "CannonUpgrade" },
        { "CapstanUpgrade",     "CapstonUpgrade" },
        { "PumpUpgrade",        "IC_pump_upgrade" },
        { "CorkUpgrade",        "ICC_CorkUpgrade" },
        { "SteeringWheelUpgrade","SteeringWheelUpgrade" },
        { "BackpackUpgrade",    "backpack_icon" },

        // ===== Toplar =====
        { "VehicleCannon",      "cannon" },
        { "VehicleSwivelCannon","swivel_cannon" },
        { "BasicSwivelCannon",  "SwivelUpgrade" },

        // ===== Kaynaklar =====
        { "Wood",               "Wood" },
        { "FuseLighter",        "flint_stick_fuse_lighter" },
        { "Paddle",             "IC_Paddle" },
    };

    // ===== WIC ile PNG yükle =====
    static ID3D11ShaderResourceView* LoadPng(ID3D11Device* device,
        const std::wstring& path) {
        IWICImagingFactory* factory = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
            CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
        if (FAILED(hr) || !factory) return nullptr;

        IWICBitmapDecoder* decoder = nullptr;
        hr = factory->CreateDecoderFromFilename(path.c_str(), nullptr,
            GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
        if (FAILED(hr) || !decoder) { factory->Release(); return nullptr; }

        IWICBitmapFrameDecode* frame = nullptr;
        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr) || !frame) {
            decoder->Release(); factory->Release(); return nullptr;
        }

        IWICFormatConverter* converter = nullptr;
        factory->CreateFormatConverter(&converter);
        converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

        UINT w = 0, h = 0;
        converter->GetSize(&w, &h);
        if (w == 0 || h == 0) {
            converter->Release(); frame->Release();
            decoder->Release(); factory->Release();
            return nullptr;
        }

        std::vector<BYTE> pixels(w * h * 4);
        converter->CopyPixels(nullptr, w * 4, (UINT)pixels.size(), pixels.data());

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = w;
        desc.Height = h;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = pixels.data();
        data.SysMemPitch = w * 4;

        ID3D11Texture2D* texture = nullptr;
        hr = device->CreateTexture2D(&desc, &data, &texture);

        ID3D11ShaderResourceView* srv = nullptr;
        if (SUCCEEDED(hr) && texture) {
            device->CreateShaderResourceView(texture, nullptr, &srv);
            texture->Release();
        }

        converter->Release();
        frame->Release();
        decoder->Release();
        factory->Release();

        return srv;
    }

    // ===== Tüm ikonları yükle =====
    void Init(ID3D11Device* device, const std::string& iconFolder) {
        for (const auto& kv : ICON_MAP) {
            const std::string& className = kv.first;
            const std::string& fileName = kv.second;

            std::wstring path = std::wstring(iconFolder.begin(), iconFolder.end())
                + L"\\"
                + std::wstring(fileName.begin(), fileName.end())
                + L".png";

            auto* srv = LoadPng(device, path);
            if (srv) {
                g_icons[className] = srv;
            }
        }
    }

    void Shutdown() {
        for (auto& kv : g_icons) {
            if (kv.second) kv.second->Release();
        }
        g_icons.clear();
    }

    ID3D11ShaderResourceView* GetIcon(const std::string& itemName) {
        auto it = g_icons.find(itemName);
        if (it == g_icons.end()) return nullptr;
        return it->second;
    }
}
