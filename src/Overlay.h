#pragma once
#include <Windows.h>

namespace Overlay {
    extern HWND hwnd;

    bool Create(HINSTANCE hInstance);
    void Destroy();
}