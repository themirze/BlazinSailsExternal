#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "UE4.h"

namespace Actors {

    struct Character {
        uintptr_t actor;
        uintptr_t rootComponent;
        float health;
        float maxHealth;
        bool isDead;
        bool isEnemy;
        bool isLocalPlayer;
        char name[64];
    };

    struct Item {
        uintptr_t actor;
        int32_t classIdx;
        std::string className;
        FVector worldPos;
    };

    std::vector<Character> GetCharacters();
    std::vector<Item> GetItems();
}
