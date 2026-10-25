# Changelog

## 0.2.4
* Fix: haptics could go dead after a few game restarts. The haptics now run in their own audio session (Windows was keeping a muted or lowered game session from earlier launches), and the session volume is put back to 100% if anything turns it down.
* The game's rumble is only replaced while the controller is really playing the haptics. If the stream stalls or the device changes, the game's own rumble comes back and the haptics restart by themselves.
* With more than one controller audio device (a second pad, DSX or DSZ, a replugged pad), the mod uses the one that belongs to the controller the game opened.
* Fix: the 435 cutscene marker lines in `GearsHaptics_events.ini` were never read ("unknown preset" in the log), so sound haptics did not pause in cutscenes. They work now.
* The log shows the Wireless Controller device volume (info only, the haptics play even at 0%).
* Sound haptics no longer stall for a moment after the stream restarts.
* Development diagnostics (GameInput census, audio tap) are off by default, `GameInputCensus` and `AudioTap` in `[Diag]`.
* The connect chime plays once per launch, not on every restart of the stream.

## 0.2.3
* First public release.
* Diagnostic logging and the name scan are off by default.
* Windows build script (`build.bat`).

## 0.2.0 to 0.2.2
* Sustained effects: Lancer per shot, chainsaw rev, cut and duel, low health heartbeat, burning.
* Renamed to Gears Haptics. Settings files are now `GearsHaptics.ini` and `GearsHaptics_events.ini`, and older settings files are carried over.

## 0.1.8
* Strength per kind of effect in `[Sound]`, pulse shapes in `[Presets]`.
* Sound haptics pause during cutscenes.
* Softer boots by default.
* Getting hurt, going down, rolls, cover slides, vaults and pickups.

## 0.1.7
* Sound haptics from about 265 of the player's own Wwise sounds: boots, guns, reloads, menus, world.

## 0.1.6
* Response curve so the game's quiet rumble is felt, stronger click on sharp hits.
* Sound name scan for mapping the game's sounds.

## 0.1.5
* First working build: the game's rumble plays on the DualSense haptics.
