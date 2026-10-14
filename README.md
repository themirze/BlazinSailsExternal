# BlazingSailsESP

External overlay ESP / aim-assist for **Blazing Sails**, written in C++20 using Direct3D 11 and [Dear ImGui](https://github.com/ocornut/imgui). It renders a transparent overlay window on top of the game and reads game data through an external memory reader.

> ⚠️ **Disclaimer** — This project is for **educational purposes only**. Using it online violates the game's Terms of Service and may result in a ban. Use at your own risk; the author is not responsible for any consequences.

## Features

- **Character ESP** — box, name, distance, health, team and dead indicators, with configurable max distance.
- **Item ESP** — chests, weapons, ammo, barrels, upgrades, cannons, resources, paddles and more.
- **Aimlock** — smooth aim assist with selectable trigger keys (right/left mouse, CTRL, SHIFT, ALT) and adjustable smoothing / max step.
- **Magic Bullet** — input- and write-based variants with configurable smoothness and cooldown.
- **Triggerbot** — auto-fires when the crosshair is over an enemy while a trigger key is held (head-only option, delay and FOV radius).
- **Bone targeting** — head / chest / feet with per-bone offset tuning.
- **FOV circle** and full color customization.
- ImGui menu with tabbed interface and adjustable UI scale.

## Controls

| Key | Action |
| --- | ------ |
| `INSERT` | Toggle the menu |
| `DELETE` | Exit the overlay |

## Building

Requirements:

- **Visual Studio** with the **MSVC `v145`** platform toolset (C++20 support required).
- **Windows SDK 10.0**.
- Windows 10/11, x64 recommended.

> The project references include paths and an asset path relative to `C:\BlazingSailsESP`. If you clone it elsewhere, update `AdditionalIncludeDirectories` in `BlazingSailsESP.vcxproj` and the icon path in `src\main.cpp`.

Steps:

1. Open `BlazingSailsESP.vcxproj` in Visual Studio.
2. Select the `Release | x64` configuration.
3. Build (`Ctrl+Shift+B`).

The output executable is written to `x64\Release\`.

## Usage

1. Start **Blazing Sails** first.
2. Run the built `BlazingSailsESP.exe`.
3. The overlay attaches automatically; press `INSERT` to open the menu and configure features.
4. Press `DELETE` to close.

## Project structure

```
src/                  Application source code
├── main.cpp          Entry point, D3D11 + ImGui setup, main loop
├── Overlay.cpp/.h    Transparent click-through window
├── Memory.cpp/.h     External process memory reader/writer
├── Game.cpp/.h       Game attach + global pointer reading
├── Offsets.h         Game offsets
├── UE4.h             Unreal Engine 4 structs and math helpers
├── Actors.cpp/.h     Actor enumeration
├── ESP*.cpp/.h       ESP rendering (characters + items)
├── Aimbot.cpp/.h     Aimlock logic
├── MagicBullet.cpp/.h
├── Triggerbot.cpp/.h
├── Menu.cpp/.h       ImGui menu
├── Theme.cpp/.h      Menu styling
├── Draw.cpp/.h       Drawing helpers
├── Config.h          Runtime settings
└── Assets.cpp/.h     Icon loading
assets/               Overlay assets (icons)
vendor/imgui/         Dear ImGui (third-party)
```

## License

Provided as-is for educational use. No warranty.
