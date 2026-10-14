#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "Overlay.h"
#include "Menu.h"
#include "ESP.h"
#include "Game.h"
#include "Config.h"
#include "Offsets.h"
#include "UE4.h"
#include "Assets.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// ===== D3D11 Globals =====
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
static bool                     g_Running = true;

// ===== D3D11 init =====
static bool InitD3D11(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
    scd.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevels, 2, D3D11_SDK_VERSION,
        &scd, &g_pSwapChain, &g_pDevice, &featureLevel, &g_pContext
    );
    if (FAILED(hr)) return false;

    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (pBackBuffer) {
        g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
        pBackBuffer->Release();
    }
    return true;
}

// ===== ImGui init =====
static void InitImGui() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigDebugHighlightIdConflicts = false;

    Menu::Init();

    ImGui_ImplWin32_Init(Overlay::hwnd);
    ImGui_ImplDX11_Init(g_pDevice, g_pContext);

    Assets::Init(g_pDevice, "C:\\BlazingSailsESP\\assets\\icons");
}

// ===== Ana dongu =====
static void MainLoop() {
    bool insertPressed = false;
    bool deletePressed = false;
    ULONGLONG lastGlobalRead = 0;

    while (g_Running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) g_Running = false;
        }

        // ===== GLOBAL KLAVYE =====
        if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
            if (!insertPressed) {
                insertPressed = true;
                Config::menuOpen = !Config::menuOpen;

                LONG_PTR exStyle = GetWindowLongPtrW(Overlay::hwnd, GWL_EXSTYLE);
                if (Config::menuOpen)
                    exStyle &= ~WS_EX_TRANSPARENT;
                else
                    exStyle |= WS_EX_TRANSPARENT;
                SetWindowLongPtrW(Overlay::hwnd, GWL_EXSTYLE, exStyle);
            }
        }
        else {
            insertPressed = false;
        }

        if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
            if (!deletePressed) {
                deletePressed = true;
                g_Running = false;
                PostQuitMessage(0);
            }
        }
        else {
            deletePressed = false;
        }

        // ===== OYUNA BAGLAN =====
        if (!Game::IsAttached())
            Game::Attach();

        // ===== GLOBAL POINTER'LARI PERIYODIK OKU =====
        if (Game::IsAttached()) {
            ULONGLONG now = GetTickCount64();
            if (now - lastGlobalRead > 5000) {
                Game::ReadGlobals();
                lastGlobalRead = now;
            }
        }

        // ===== IMGUI =====
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ESP::Render();
        Menu::Render();

        ImGui::Render();

        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
        g_pContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }
}

// ===== Giris =====
int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow) {

    if (!Overlay::Create(hInstance)) {
        MessageBoxA(nullptr, "Overlay olusturulamadi", "Hata", MB_OK);
        return 1;
    }
    if (!InitD3D11(Overlay::hwnd)) {
        MessageBoxA(nullptr, "D3D11 baslatilamadi", "Hata", MB_OK);
        return 1;
    }
    InitImGui();

    MainLoop();

    Assets::Shutdown();

    Game::Detach();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain)        g_pSwapChain->Release();
    if (g_pContext)          g_pContext->Release();
    if (g_pDevice)           g_pDevice->Release();
    Overlay::Destroy();

    return 0;
}
