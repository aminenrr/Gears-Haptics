#!/bin/sh
# Cross-builds winmm.dll with zig (ziglang.org, or pip install ziglang).
set -e
cd "$(dirname "$0")"
if command -v zig >/dev/null 2>&1; then ZIG=zig; else ZIG="python3 -m ziglang"; fi
M=third_party/minhook
mkdir -p out
$ZIG cc -target x86_64-windows-gnu -shared -O2 -s -o out/winmm.dll \
  -I$M/include src/main.c src/winmm_stubs.c src/census_stubs.c \
  $M/src/hook.c $M/src/buffer.c $M/src/trampoline.c $M/src/hde/hde64.c \
  -lole32 -luuid -lkernel32 -luser32 -lpsapi
$ZIG cc -O2 -o out/mkini tools/mkini.c
./out/mkini
ls -la out/winmm.dll out/GearsHaptics.ini out/GearsHaptics_events.ini
