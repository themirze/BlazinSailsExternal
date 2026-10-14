#include "Actors.h"
#include "Game.h"
#include <cmath> 
#include "Memory.h"
#include "ItemClasses.h"
#include "Offsets.h"
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <algorithm>

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
    // Item'ları topla (500ms cache + tek seferlik class tarama debug)
    // ============================================================
    std::vector<Item> GetItems() {
        std::vector<Item> result;
        if (!Game::GWorld) return result;

        uintptr_t level = Memory::Read<uintptr_t>(Game::GWorld + 0x30);
        if (!level) return result;

        uintptr_t data = Memory::Read<uintptr_t>(level + 0x98);
        int32_t count = Memory::Read<int32_t>(level + 0xA0);
        if (!data || count <= 0 || count > 100000) return result;

    // ============================================================
    // ★★★ DEBUG: TÜM BENZERSİZ CLASS İSİMLERİNİ LİSTELE ★★★
    // ============================================================
    static bool s_dumpedClasses = false;
    static bool s_dumpKeyPressed = false;

    // F8'e basıldığında yeniden listele
    bool f8Down = (GetAsyncKeyState(VK_F8) & 0x8000) != 0;

    if (!s_dumpedClasses || (f8Down && !s_dumpKeyPressed)) {
        s_dumpKeyPressed = f8Down;

        printf("\n");
        printf("=====================================================================\n");
        printf("  TUM BENZERSIZ ACTOR CLASS ISIMLERI (F8 = yenile)\n");
        printf("=====================================================================\n");

        std::unordered_map<std::string, int> uniqueClasses;

        for (int i = 0; i < count; i++) {
            uintptr_t actor = Memory::Read<uintptr_t>(data + (uintptr_t)i * 8);
            if (!actor || actor < 0x10000) continue;

            uintptr_t cp = Memory::Read<uintptr_t>(actor + 0x10);
            if (!cp) continue;

            int32_t idx = Memory::Read<int32_t>(cp + 0x18);
            std::string className = UE4::ReadFName(Game::GNames, idx);

            if (className.empty() || className == "?") continue;
            if (className.length() > 80) continue;

            // Sadece BP_ / ABP_ / Pickup / Item içerenler
            // (bu filtreyi kaldırırsan 1000+ class basar)
            bool interesting =
                className.find("BP_") == 0 ||
                className.find("ABP_") == 0 ||
                className.find("Pickup") != std::string::npos ||
                className.find("Item") != std::string::npos ||
                className.find("Weapon") != std::string::npos;

            if (interesting) {
                uniqueClasses[className]++;
            }
        }

        // İsme göre sıralı yazdır
        std::vector<std::pair<std::string, int>> sorted(
            uniqueClasses.begin(), uniqueClasses.end());
        std::sort(sorted.begin(), sorted.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });

        int itemCandidateCount = 0;
        for (const auto& kv : sorted) {
            printf("  %-55s (x%d)\n", kv.first.c_str(), kv.second);
            itemCandidateCount++;
        }

        printf("=====================================================================\n");
        printf("  TOPLAM: %d benzersiz class  (F8 = yenile)\n", itemCandidateCount);
        printf("=====================================================================\n\n");

        s_dumpedClasses = true;
    }

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

                // ★ Sadece class ismi tabanlı
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

    // ============================================================
    // ★ GEÇİCİ DEBUG: Projectile + PMC offset bulma (tek F9)
    // ============================================================
    void DumpProjectiles() {
        printf("\n[DUMP] F9 BASILDI!\n");
        if (!Game::GWorld) return;

        uintptr_t level = Memory::Read<uintptr_t>(Game::GWorld + 0x30);
        if (!level) return;

        uintptr_t data = Memory::Read<uintptr_t>(level + 0x98);
        int32_t count = Memory::Read<int32_t>(level + 0xA0);
        if (!data || count <= 0) return;

        int projCount = 0;

        for (int i = 0; i < count; i++) {
            uintptr_t actor = Memory::Read<uintptr_t>(data + (uintptr_t)i * 8);
            if (!actor || actor < 0x10000) continue;

            uintptr_t cp = Memory::Read<uintptr_t>(actor + 0x10);
            if (!cp) continue;

            int32_t classIdx = Memory::Read<int32_t>(cp + 0x18);
            std::string className = UE4::ReadFName(Game::GNames, classIdx);

            if (className.find("Projectile") == std::string::npos) continue;
            if (className.find("Trail") != std::string::npos) continue;

            printf("\n=====================================================================\n");
            printf("[PROJ] actor=0x%llX class=%s\n",
                (unsigned long long)actor, className.c_str());
            printf("=====================================================================\n");
            printf("TUM POINTER'LAR ve CLASS ISIMLERI:\n\n");

            for (int off = 0x100; off < 0x2000; off += 8) {
                uintptr_t ptr = Memory::Read<uintptr_t>(actor + off);
                if (ptr < 0x10000000000 || ptr > 0x7FFFFFFFFFFF) continue;

                // Pointer'ın class ismini oku
                uintptr_t ptrClass = Memory::Read<uintptr_t>(ptr + 0x10);
                std::string ptrClassName = "?";
                if (ptrClass) {
                    int32_t pcIdx = Memory::Read<int32_t>(ptrClass + 0x18);
                    ptrClassName = UE4::ReadFName(Game::GNames, pcIdx);
                }

                // İlk birkaç float'ı da oku (velocity olup olmadığını görmek için)
                float v_c4_x = Memory::Read<float>(ptr + 0xc4);
                float v_c4_y = Memory::Read<float>(ptr + 0xc4 + 4);
                float v_c4_z = Memory::Read<float>(ptr + 0xc4 + 8);

                printf("  actor+0x%04X -> 0x%llX  [%s]  | +0xc4=(%.1f, %.1f, %.1f)\n",
                    off, (unsigned long long)ptr, ptrClassName.c_str(),
                    v_c4_x, v_c4_y, v_c4_z);
            }

            printf("\n");
            projCount++;
            if (projCount >= 1) break;   // sadece 1 projectile yeter
        }

        printf("=== TOTAL %d PROJECTILES ===\n\n", projCount);
    }
}