# Gears Haptics

Real DualSense voice-coil haptics for Gears of War: E-Day on Steam, as a small `winmm.dll` mod.

> [!WARNING]
> **Story mode only, with anti-cheat turned off.** This mod hooks the game's sound engine and controller output, so it only works when the game runs **without Easy Anti-Cheat**. Do not use it in multiplayer or any online mode. Using a modded game online can get your account banned. Use it in the campaign only, at your own risk.
>
> **To play offline without anti-cheat, use [Gears of War E-Day EAC Offline Bypass](https://www.nexusmods.com/gearsofwareday/mods/4) on Nexus Mods, or set the Windows environment variable `EOS_USE_ANTICHEATCLIENTNULL` to `1` and restart your PC.**

## Contents

* [Why this mod](#why-this-mod)
* [Features](#features)
* [Requirements](#requirements)
* [Install](#install)
* [Settings](#settings)
* [Mapping more sounds](#mapping-more-sounds)
* [Files](#files)
* [Building it yourself](#building-it-yourself)
* [How it works](#how-it-works)
* [Troubleshooting](#troubleshooting)
* [Disclaimer](#disclaimer)
* [Credits](#credits)
* [License](#license)

## Why this mod

The PC version already supports most of the DualSense: adaptive triggers, gyro, lightbar and PlayStation button prompts. Sadly the biggest one, HD haptics, is not supported, and the game falls back to the old two-motor rumble. This mod adds it.

* **HD haptics instead of rumble:** on PC the game only drives the DualSense like an old two-motor pad, which feels weak and buzzy. The mod catches that rumble and plays it on the controller's left and right haptic actuators instead: a deep thump for the heavy motor, a sharper buzz for the light one, a curve that brings out the quiet rumble the game sends most of the time, and an extra click on sharp hits.
* **Feel the sound effects:** about 280 of the game's own sounds, taken straight from its Wwise sound engine, and only the ones made by you (never squad mates, enemies or music): boots (alternating left and right), landings, mantles, rolls, cover slams and slides, every shot, every part of a reload, active reload, equip, grenades, kicks, executions, getting hurt and going down, ammo and collectible pickups, doors, chests, E-holes, screen shake, the hit marker and the menus.
* **Sustained effects:** the Lancer clicks on every shot, the chainsaw grinds while you rev it and hits hard when cutting or duelling, low health beats like a heart, and burning buzzes.
* **Cutscenes stay quiet:** sound haptics pause during cutscenes and come back when you have control again.
* **Keeps everything else:** adaptive triggers, gyro, lightbar, button prompts and the rest of the game's DualSense features stay exactly as the game sets them. Only the rumble bytes are replaced.
* **Tunable:** strength per kind of effect, every pulse's shape, and every single sound in two `.ini` files.

## Requirements

* Gears of War: E-Day on Steam (Windows), **with Easy Anti-Cheat off**, campaign only.
* A DualSense or DualSense Edge controller. **Use a USB cable, or for Bluetooth use [DSZ](https://github.com/MaximumFreak/dsz) (DualSense mode) or DSX with its Bluetooth haptics. Without one of these the haptics do not work over Bluetooth.**
* Nothing plugged into the controller's headphone jack. A headset in the jack turns the haptics off.
* Steam Input off for the game, so the game talks to the DualSense directly.

## Install

1. Download the latest zip from **Releases**.
2. Extract it into the game folder, `...\steamapps\common\GoWEDay\`. The files go to `FairlightConcept\Binaries\Win64\`, next to `GoWEDay-Steam.exe`:
   * `winmm.dll` the mod
   * `GearsHaptics.ini` settings
   * `GearsHaptics_events.ini` the list of sounds you can feel
3. Connect the DualSense (USB, or Bluetooth through DSZ or DSX) and start the game with anti-cheat off. You should feel a short chime when the haptics connect.

To uninstall, delete `winmm.dll` (and the two `.ini` files if you like).

**Updating:** if you tuned your settings, keep your own `GearsHaptics.ini` and only copy the new `winmm.dll` and `GearsHaptics_events.ini`. New settings are added to an old `GearsHaptics.ini` by themselves.

## Settings

`GearsHaptics.ini`:

* `[General]` `Strength` (master, 0 to 2), `KeepNativeRumble` (also keep the game's own rumble), `Chime`
* `[Tuning]` the rumble: `LowMotorHz`, `HighMotorHz`, `LowGain`, `HighGain`, `SideBleed` (how much each motor leaks to the other side), `Curve` (lower = quiet rumble felt more), `TransientGain`, `TransientThreshold`, `EventGain` (master for all sound haptics)
* `[Sound]` strength per kind of sound haptic, 0 turns it off: `Boots`, `Guns`, `Reload`, `Melee`, `Grenade`, `Chainsaw`, `Body`, `Damage`, `Pickups`, `World`, `HUD`, `Menus`. `CutsceneMute` and `CutsceneHoldSec` control the cutscene pause.
* `[Presets]` the shape of each pulse type (`tick`, `click`, `knock`, `thump`, `buzz`) as frequency in Hz, decay in ms and length in ms
* `[Diag]` `HookWwise` (needed for sound haptics), `LogPosts` and `NameScan` for mapping new sounds

`GearsHaptics_events.ini`:

* `[Events]` one line per sound: `id=preset,gain,side,category ; name`. Side is `B` (both), `L`, `R` or `A` (alternate, used for boots). Put a `;` in front of a line to turn that sound off, or change its preset and gain.
* `cine` lines mark cutscene sounds, which pause the sound haptics.
* `[Loops]` sustained effects: `startId=group,preset,gain,side,category,periodMs,maxSec,dubMs,stop`. A loop pulses every `periodMs` until one of its stop sounds plays, the game's rumble goes quiet (`rumble`), or `maxSec` runs out. `dubMs` adds a softer second beat (the heartbeat).

## Mapping more sounds

Set `LogPosts=1` and `NameScan=1` in `[Diag]`, play for a few minutes, and the mod writes `GearsHaptics_catalog.txt` with every sound it heard, how often it played and its name. The game names your own sounds without a suffix, and others with `_SQUD` (squad), `_ENMY` (enemies), `_SHRD` (anyone), `_CINE` (cutscenes). Add the ones you want to `GearsHaptics_events.ini`. The name scan reads the game's memory for about half a minute at 1, 3 and 6 minutes and can stutter while it runs, so leave it off for normal play.

## Files

* `CHANGELOG.md` what changed in each version.
* `src/main.c` the whole mod: the `winmm.dll` proxy, controller output hooks, Wwise hooks, the haptics synth and the settings.
* `src/defaults_ini.h` the default `GearsHaptics.ini`.
* `src/events_default.h` the default sound list (`GearsHaptics_events.ini`).
* `src/winmm_stubs.c` forwards every real `winmm.dll` export to the system copy.
* `src/census_stubs.c` small hook stubs for the Wwise sound events.
* `tools/mkini.c` writes the default `.ini` files into `out/`.
* `third_party/minhook` MinHook.

## Building it yourself

You need [zig](https://ziglang.org/download/), which brings its own C compiler for Windows. Unzip it and add it to your PATH. No game files are needed.

On Windows:

```
build.bat
```

On Linux or macOS:

```
./build.sh
```

Both write `out\winmm.dll`, `out\GearsHaptics.ini` and `out\GearsHaptics_events.ini`. Copy them next to `GoWEDay-Steam.exe`.

## How it works

* The game loads `winmm.dll` from its own folder first, so the mod sits there and passes every real call on to the system `winmm.dll`.
* The game writes its own DualSense output reports with `WriteFile`. The mod reads the two rumble bytes from each report (both the old and the newer "compatible vibration" flags), zeroes them and passes the report on, so triggers, lights and everything else are untouched.
* The rumble values, after the response curve, drive two sine waves on the DualSense's USB audio device (`Wireless Controller`, 4 channels), whose channels 3 and 4 move the left and right actuators.
* Wwise is built into the game. The mod hooks its `PostEvent` calls and checks each sound id against the list in `GearsHaptics_events.ini`; only listed sounds are felt. Each hit starts a short decaying pulse on top of the rumble, up to 8 at once. Nothing captures or listens to the game's audio output.

## Troubleshooting

* Look in `GearsHaptics.log` next to the game's exe. It says when the DualSense audio device is found and when the game opens the controller.
* No haptics at all on USB: check the cable, unplug anything from the headphone jack, and turn off Bluetooth on the PC (or unpair the controller) so it does not stay connected wirelessly.
* No haptics on Bluetooth: make sure DSZ is in DualSense mode (or DSX has Bluetooth haptics on) before starting the game.
* No sound haptics but rumble works: make sure `HookWwise=1`.
* The game crashes about 15 seconds after launch: set `HookWwise=0` and report it with your log.
* Haptics stop after a while or after a few launches: update to 0.2.4 or newer. If it still happens, check that the Wireless Controller device is not muted in Windows sound settings, and send `GearsHaptics.log` from that session (it is overwritten on every launch).
* Too strong or too weak: change `Strength`, or a single category in `[Sound]`.

## Disclaimer

This project is not affiliated with or endorsed by Microsoft, Xbox Game Studios, The Coalition, People Can Fly or Sony. It contains no game files and only ships its own DLL and settings. You need your own legitimate copy of the game. Use it in the campaign with anti-cheat off, at your own risk.

Gears of War is a trademark of Microsoft. PlayStation and DualSense are trademarks of Sony Interactive Entertainment. They are named here only to describe what the mod does.

## Credits

* [MinHook](https://github.com/TsudaKageyu/minhook) by Tsuda Kageyu, BSD 2-Clause.
* [zig](https://ziglang.org) for building.

## License

MIT, see `LICENSE`.
