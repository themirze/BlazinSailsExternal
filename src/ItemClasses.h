#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ItemClasses {

    inline const std::unordered_map<std::string, std::string> CLASS_MAP = {
        // ===== Silahlar =====
        {"BP_PickupWeaponFlintlock_C",        "Flintlock"},
        {"BP_PickupWeaponBlunderbussNew_C",   "Blunderbuss"},
        {"BP_PickupWeaponCrossbow_C",         "Crossbow"},
        {"BP_PickupWeaponMusket_C",           "Musket"},
        {"BP_PickupWeaponClub_C",             "Club"},
        {"BP_PickupWeaponRepeater_C",         "Repeater"},
        {"BP_PickupWeaponDoubleBarrel_C",     "DoubleBarrel"},
        {"BP_PickupWeaponhealingCrossbow_C",  "HealingCrossbow"},
        {"BP_PickupWeaponCutlass_C",          "Cutlass"},
        {"BP_PickupWeaponSniper2_C",          "Sniper"},
        {"BP_PickupWeaponPickaxe_C",          "Pickaxe"},
        {"BP_PickupWeaponShovel_C",           "Shovel"},
        {"BP_PickupWeaponButchersKnife_C",    "ButchersKnife"},
        {"BP_PickupWeaponDualFlintlock_C",    "DualFlintlock"},
        {"BP_PickupWeaponQuadFlintlock_C",    "QuadFlintlock"},
        {"BP_PickupWeaponDualQuadFlintlock_C","DualQuadFlintlock"},
        {"BP_PickupWeaponDragonPistol_C",     "DragonPistol"},
        {"BP_PickupWeaponHalfBarrel_C",       "HalfBarrel"},
        {"BP_PickupWeaponBombLauncher_C",     "BombLauncher"},
        {"BP_WeaponBrokenBottle_C",           "BrokenBottle"},

        // ===== Sandıklar =====
        {"BP_Lootcrate_C",                    "Lootcrate"},
        {"BP_TreasureChest_C",                "TreasureChest"},
        {"BP_AmmoBox_C",                      "AmmoBox"},
        {"BP_WoodCrate_C",                    "WoodCrate"},
        {"BP_WoodBarrel_C",                   "WoodBarrel"},
        {"BP_CarryChestBase_C",               "CarryChest"},
        {"BP_PickupExplosiveBarrel_C",        "ExplosiveBarrel"},

        // ===== Mermiler =====
        {"BP_PickupArrows_C",                 "Arrows"},
        {"BP_PickupSmallShot_C",              "SmallShot"},
        {"BP_PickupHeavyShot_C",              "HeavyShot"},
        {"BP_PickupCannonBalls_C",            "CannonBalls"},
        {"BP_PickupFishAmmo_C",               "FishAmmo"},
        {"BP_PickupAntiCannon_C",             "AntiCannon"},   // ★ YENİ
        {"BP_PickupAntiSail_C",               "AntiSail"},     // ★ YENİ


        // ===== Upgrade'ler =====
        {"BP_PickupCorkUpgrade_C",            "CorkUpgrade"},
        {"BP_PickupCannonUpgrade_C",          "CannonUpgrade"},
        {"BP_PickupCapstanUpgrade_C",         "CapstanUpgrade"},
        {"BP_PickupPumpUpgrade_C",            "PumpUpgrade"},
        {"BP_PickupSteeringWheelUpgrade_C",   "SteeringWheelUpgrade"},
        {"BP_PickupBackpackUpgrade_C",        "BackpackUpgrade"},

        // ===== Gemi Topları =====
        {"BP_VehicleCannon_C",                "VehicleCannon"},
        {"BP_VehicleSwivelCannon_C",          "VehicleSwivelCannon"},
        {"BP_PickupBasicSwivelCannon_C",      "BasicSwivelCannon"},

        // ===== Kaynaklar =====
        {"BP_PickupWood_C",                   "Wood"},
        {"BP_PickupFuseLighter_C",            "FuseLighter"},
        {"BP_PickupPaddle_C",                 "Paddle"},
    };

    inline const char* GetNameFromClassName(const std::string& className) {
        auto it = CLASS_MAP.find(className);
        return (it != CLASS_MAP.end()) ? it->second.c_str() : nullptr;
    }

    // GetIcon — kategorilere göre
    inline const char* GetIcon(const std::string& name) {
        if (name.find("Cannon") != std::string::npos) return "[CANNON]";
        if (name.find("Chest") != std::string::npos) return "[CHEST]";
        if (name.find("Lootcrate") != std::string::npos) return "[LOOT]";
        if (name.find("Ammo") != std::string::npos) return "[AMMO]";
        if (name.find("Arrows") != std::string::npos) return "[AMMO]";
        if (name.find("SmallShot") != std::string::npos) return "[AMMO]";
        if (name.find("HeavyShot") != std::string::npos) return "[AMMO]";
        if (name.find("CannonBalls") != std::string::npos) return "[AMMO]";
        if (name.find("Barrel") != std::string::npos) return "[BARREL]";
        if (name.find("Upgrade") != std::string::npos) return "[UPGRADE]";
        if (name.find("Wood") != std::string::npos) return "[WOOD]";
        if (name.find("Paddle") != std::string::npos) return "[PADDLE]";
        return "[ITEM]";
    }

    inline const char* GetIcon(int32_t /*idx*/) { return "[ITEM]"; }

    // ===== Kategoriler =====
    enum class Category {
        Chest, Weapon, Ammo, Barrel, Upgrade,
        Cannon, Resource, Paddle, Other
    };

    inline Category GetCategory(const std::string& name) {
        if (name.find("Chest") != std::string::npos) return Category::Chest;
        if (name.find("Lootcrate") != std::string::npos) return Category::Chest;
        if (name.find("Crate") != std::string::npos) return Category::Chest;
        if (name.find("AmmoBox") != std::string::npos) return Category::Chest;
        if (name.find("CarryChest") != std::string::npos) return Category::Chest;

        if (name.find("Flintlock") != std::string::npos) return Category::Weapon;
        if (name.find("Blunderbuss") != std::string::npos) return Category::Weapon;
        if (name.find("Crossbow") != std::string::npos) return Category::Weapon;
        if (name.find("Musket") != std::string::npos) return Category::Weapon;
        if (name.find("Cutlass") != std::string::npos) return Category::Weapon;
        if (name.find("Club") != std::string::npos) return Category::Weapon;
        if (name.find("Repeater") != std::string::npos) return Category::Weapon;
        if (name.find("DoubleBarrel") != std::string::npos) return Category::Weapon;
        if (name.find("HalfBarrel") != std::string::npos) return Category::Weapon;
        if (name.find("Sniper") != std::string::npos) return Category::Weapon;
        if (name.find("Shovel") != std::string::npos) return Category::Weapon;
        if (name.find("Pickaxe") != std::string::npos) return Category::Weapon;
        if (name.find("ButchersKnife") != std::string::npos) return Category::Weapon;
        if (name.find("DragonPistol") != std::string::npos) return Category::Weapon;
        if (name.find("BombLauncher") != std::string::npos) return Category::Weapon;
        if (name.find("BrokenBottle") != std::string::npos) return Category::Weapon;

        if (name.find("Arrows") != std::string::npos) return Category::Ammo;
        if (name.find("SmallShot") != std::string::npos) return Category::Ammo;
        if (name.find("HeavyShot") != std::string::npos) return Category::Ammo;
        if (name.find("CannonBalls") != std::string::npos) return Category::Ammo;
        if (name.find("FishAmmo") != std::string::npos) return Category::Ammo;
        if (name.find("AntiCannon") != std::string::npos) return Category::Ammo;   // ★ YENİ
        if (name.find("AntiSail") != std::string::npos) return Category::Ammo;     // ★ YENİ

        if (name.find("Barrel") != std::string::npos) return Category::Barrel;

        if (name.find("Upgrade") != std::string::npos) return Category::Upgrade;

        if (name.find("Cannon") != std::string::npos) return Category::Cannon;

        if (name.find("Wood") != std::string::npos) return Category::Resource;
        if (name.find("FuseLighter") != std::string::npos) return Category::Resource;

        if (name.find("Paddle") != std::string::npos) return Category::Paddle;

        return Category::Other;
    }
}