#include "Overlay.h"
#include "Config.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Overlay {

    HWND hwnd = nullptr;
    static WNDCLASSEXW wc = {};

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    bool Create(HINSTANCE hInstance) {
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
        wc.lpszClassName = L"BlazingOverlayClass";

        if (!RegisterClassExW(&wc)) return false;

        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);

        // Baslangicta WS_EX_TRANSPARENT YOK (menu acik)
        hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
            L"BlazingOverlayClass", L"Blazing Overlay", WS_POPUP,
            0, 0, sw, sh,
            nullptr, nullptr, hInstance, nullptr
        );

        if (!hwnd) return false;

        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        return true;
    }

    void Destroy() {
        if (hwnd) { DestroyWindow(hwnd); hwnd = nullptr; }
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }
}