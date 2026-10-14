#include "Actors.h"
#include "Game.h"
#include <cmath>
#include "Memory.h"
#include "ItemClasses.h"
#include "Offsets.h"
#include <cstring>
#include <unordered_map>

#include "UE4.h"

namespace Actors {

    // ============================================================
    // Karakter class filtresi
    // ============================================================
    static bool IsCharacterClass(const std::string& className) {
        if (className.empty() || className.length() > 64) return false;
        if (className.find("ABP_") == 0) return false;
        if (className.find("BP_") != 0) return false;
        if (className.find("Character") == std::string::npos) return false;
        if (className.find("Ref") != std::string::npos) return false;
        if (className.find("Movement") != std::string::npos) return false;
        if (className.find("Inventory") != std::string::npos) return false;
        if (className.find("Controller") != std::string::npos) return false;
        if (className.find("Camera") != std::string::npos) return false;
        return true;
    }

    // ============================================================
    // CACHE
    // ============================================================
    static std::vector<uintptr_t> s_charActors;
    static std::vector<uintptr_t> s_itemActors;
    static DWORD s_lastCharScan = 0;
    static DWORD s_lastItemScan = 0;
    static std::unordered_map<int32_t, std::string> s_classNameCache;

    static const std::string& GetCachedClassName(int32_t classIdx) {
        auto it = s_classNameCache.find(classIdx);
        if (it == s_classNameCache.end()) {
            std::string name = UE4::ReadFName(Game::GNames, classIdx);
            it = s_classNameCache.emplace(classIdx, name).first;
        }
        return it->second;
    }

    // ============================================================
    // Karakterleri topla (200ms cache)
    // ============================================================
    std::vector<Character> GetCharacters() {
        std::vector<Character> result;
        if (!Game::GWorld) return result;

        uintptr_t level = Memory::Read<uintptr_t>(Game::GWorld + 0x30);
        if (!level) return result;

        uintptr_t data = Memory::Read<uintptr_t>(level + 0x98);
        int32_t count = Memory::Read<int32_t>(level + 0xA0);
        if (!data || count <= 0 || count > 100000) return result;

        DWORD now = GetTickCount();
        if (now - s_lastCharScan > 200) {
            s_charActors.clear();
            for (int i = 0; i < count; i++) {
                uintptr_t actor = Memory::Read<uintptr_t>(data + (uintptr_t)i * 8);
                if (!actor || actor < 0x10000) continue;

                uintptr_t cp = Memory::Read<uintptr_t>(actor + 0x10);
                if (!cp) continue;

                int32_t classIdx = Memory::Read<int32_t>(cp + 0x18);
                const std::string& className = GetCachedClassName(classIdx);
                if (!IsCharacterClass(className)) continue;

                s_charActors.push_back(actor);
            }
            s_lastCharScan = now;
        }

        for (uintptr_t actor : s_charActors) {
            int32_t hp = Memory::Read<int32_t>(actor + Offsets::Char_Health);
            int32_t mhp = Memory::Read<int32_t>(actor + Offsets::Char_MaxHealth);

            Character c{};
            c.actor = actor;
            c.health = (float)hp;
            c.maxHealth = (float)mhp;
            c.isDead = Memory::Read<bool>(actor + Offsets::Char_IsDead);
            c.isEnemy = Memory::Read<bool>(actor + Offsets::Char_IsEnemy);
            c.isLocalPlayer = (actor == Game::localPawn);
            c.rootComponent = Memory::Read<uintptr_t>(
                actor + Offsets::Actor_RootComponent);

            std::string playerName = UE4::ReadFString(actor + Offsets::Char_UserName);
            if (playerName.empty() || playerName.length() > 32)
                playerName = "Unknown";
            strncpy_s(c.name, playerName.c_str(), sizeof(c.name) - 1);

            result.push_back(c);
        }
        return result;
    }

    // ============================================================
    // Item'ları topla (500ms cache)
    // ============================================================
    std::vector<Item> GetItems() {
        std::vector<Item> result;
        if (!Game::GWorld) return result;

        uintptr_t level = Memory::Read<uintptr_t>(Game::GWorld + 0x30);
        if (!level) return result;

        uintptr_t data = Memory::Read<uintptr_t>(level + 0x98);
        int32_t count = Memory::Read<int32_t>(level + 0xA0);
        if (!data || count <= 0 || count > 100000) return result;

        DWORD now = GetTickCount();
        if (now - s_lastItemScan > 500) {
            s_itemActors.clear();
            for (int i = 0; i < count; i++) {
                uintptr_t actor = Memory::Read<uintptr_t>(data + (uintptr_t)i * 8);
                if (!actor || actor < 0x10000) continue;

                uintptr_t cp = Memory::Read<uintptr_t>(actor + 0x10);
                if (!cp) continue;

                int32_t idx = Memory::Read<int32_t>(cp + 0x18);
                const std::string& className = GetCachedClassName(idx);

                // Sadece class ismi tabanlı
                const char* name = ItemClasses::GetNameFromClassName(className);
                if (!name) continue;

                s_itemActors.push_back(actor);
            }
            s_lastItemScan = now;
        }

        // Cached aktörler için taze pozisyon
        for (uintptr_t actor : s_itemActors) {
            uintptr_t cp = Memory::Read<uintptr_t>(actor + 0x10);
            if (!cp) continue;

            int32_t idx = Memory::Read<int32_t>(cp + 0x18);
            const std::string& className = GetCachedClassName(idx);
            const char* name = ItemClasses::GetNameFromClassName(className);
            if (!name) continue;

            uintptr_t root = Memory::Read<uintptr_t>(
                actor + Offsets::Actor_RootComponent);
            if (!root) continue;

            FVector pos = Memory::Read<FVector>(
                root + Offsets::Scene_RelativeLocation);
            if (pos.X == 0.0f && pos.Y == 0.0f) continue;

            Item it{};
            it.actor = actor;
            it.classIdx = idx;
            it.className = name;
            it.worldPos = pos;
            result.push_back(it);
        }
        return result;
    }
}
