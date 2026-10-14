#pragma once
#include <cmath>
#include <imgui.h>
#include <cstdint>
#include <string>
#include "Memory.h"

struct FVector {
    float X = 0, Y = 0, Z = 0;
    FVector() = default;
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}

    FVector operator-(const FVector& v) const {
        return FVector(X - v.X, Y - v.Y, Z - v.Z);
    }
    FVector operator+(const FVector& v) const {
        return FVector(X + v.X, Y + v.Y, Z + v.Z);
    }
    // Skaler çarpma
    FVector operator*(float s) const {
        return FVector(X * s, Y * s, Z * s);
    }
    // Skaler bölme
    FVector operator/(float s) const {
        if (s == 0.0f) return FVector();
        return FVector(X / s, Y / s, Z / s);
    }

    float Dot(const FVector& v) const {
        return X * v.X + Y * v.Y + Z * v.Z;
    }
    float Length() const {
        return sqrtf(X * X + Y * Y + Z * Z);
    }

    // Cross product
    FVector Cross(const FVector& v) const {
        return FVector(
            Y * v.Z - Z * v.Y,
            Z * v.X - X * v.Z,
            X * v.Y - Y * v.X
        );
    }

    // Normalize
    FVector Normalized() const {
        float len = Length();
        if (len < 0.0001f) return FVector();
        return FVector(X / len, Y / len, Z / len);
    }
};

struct FRotator {
    float Pitch = 0, Yaw = 0, Roll = 0;
};

struct FMinimalViewInfo {
    FVector  Location;              // 0x00
    FRotator Rotation;              // 0x0C
    float    FOV;                   // 0x18
    float    DesiredFOV;            // 0x1C
    float    OrthoWidth;            // 0x20
    float    OrthoNearClipPlane;    // 0x24
    float    OrthoFarClipPlane;     // 0x28
    float    AspectRatio;           // 0x2C
};

struct FCameraCacheEntry {
    float TimeStamp = 0.0f;
    uint8_t _pad[0xC] = {};
    FMinimalViewInfo POV;
};

namespace UE4 {

    inline std::string ReadFName(uintptr_t fnamePoolBase, int32_t nameIndex) {
        if (!fnamePoolBase || nameIndex < 0 || nameIndex > 10000000) return "?";
        int32_t blockIndex = nameIndex >> 16;
        int32_t offsetInBlock = nameIndex & 0xFFFF;
        uintptr_t blockPtr = Memory::Read<uintptr_t>(
            fnamePoolBase + 0x10 + (uintptr_t)blockIndex * 8);
        if (!blockPtr) return "?";
        uintptr_t entryAddr = blockPtr + (uintptr_t)offsetInBlock * 2;
        uint16_t header = Memory::Read<uint16_t>(entryAddr);
        bool bIsWide = (header & 0x01) != 0;
        int32_t len = (header >> 6) & 0x3FF;
        if (len == 0 || len > 1024) return "?";
        std::string result;
        result.reserve(len);
        if (bIsWide) {
            for (int i = 0; i < len; i++) {
                uint16_t wchar = Memory::Read<uint16_t>(entryAddr + 2 + (uintptr_t)i * 2);
                if (wchar == 0) break;
                result += (char)(wchar & 0xFF);
            }
        }
        else {
            for (int i = 0; i < len; i++) {
                char ch = Memory::Read<char>(entryAddr + 2 + (uintptr_t)i);
                if (ch == 0) break;
                result += ch;
            }
        }
        return result;
    }

    // FString okuyucu (UTF-16 → std::string)
    inline std::string ReadFString(uintptr_t fstringAddr) {
        if (!fstringAddr) return "";

        uintptr_t dataPtr = Memory::Read<uintptr_t>(fstringAddr + 0x00);
        int32_t count = Memory::Read<int32_t>(fstringAddr + 0x08);

        if (!dataPtr || count <= 0 || count > 128) return "";

        std::string result;
        result.reserve(count);
        for (int i = 0; i < count; i++) {
            uint16_t wc = Memory::Read<uint16_t>(dataPtr + (uintptr_t)i * 2);
            if (wc == 0) break;
            // UTF-16 → ASCII (sadece latin karakterler için)
            result += (char)(wc & 0xFF);
        }
        return result;
    }

    inline std::string GetObjectName(uintptr_t objectPtr, uintptr_t gNames) {
        if (!objectPtr) return "?";
        int32_t nameIndex = Memory::Read<int32_t>(objectPtr + 0x18);
        return ReadFName(gNames, nameIndex);
    }

    // ============================================================
    // WorldToScreen — BASİT FORMÜL (matrix yok, origin shift'ten az etkilenir)
    // ============================================================
    inline bool WorldToScreen(FVector world, ImVec2& out, FMinimalViewInfo pov, int sw, int sh) {
        const float D2R = 3.1415926535897932f / 180.0f;
        float pitch = pov.Rotation.Pitch * D2R;
        float yaw = pov.Rotation.Yaw * D2R;
        float roll = pov.Rotation.Roll * D2R;

        float SP = sinf(pitch), CP = cosf(pitch);
        float SY = sinf(yaw), CY = cosf(yaw);
        float SR = sinf(roll), CR = cosf(roll);

        FVector delta = world - pov.Location;
        FVector xAxis(CP * CY, CP * SY, SP);
        FVector yAxis(SR * SP * CY - CR * SY, SR * SP * SY + CR * CY, -SR * CP);
        FVector zAxis(-(CR * SP * CY + SR * SY), CY * SR - CR * SP * SY, CR * CP);

        float x = delta.Dot(yAxis);
        float y = delta.Dot(zAxis);
        float z = delta.Dot(xAxis);

        if (z < 1.0f) return false;

        float fov = pov.FOV;
        if (fov <= 0.0f || fov > 180.0f) fov = 110.0f;

        float fovRad = fov * D2R;
        float tanHalf = tanf(fovRad * 0.5f);
        float aspect = (float)sw / (float)sh;

        out.x = (sw * 0.5f) * (1.0f + x / (z * tanHalf));
        out.y = (sh * 0.5f) * (1.0f - y * aspect / (z * tanHalf));

        return true;
    }
}