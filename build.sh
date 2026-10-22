#!/bin/sh
# Cross-builds winmm.dll with zig (pip install ziglang).
set -e
cd "$(dirname "$0")"
M=third_party/minhook
mkdir -p out
python3 -m ziglang cc -target x86_64-windows-gnu -shared -O2 -s -o out/winmm.dll \
  -I$M/include src/main.c src/winmm_stubs.S src/census_stubs.S \
  $M/src/hook.c $M/src/buffer.c $M/src/trampoline.c $M/src/hde/hde64.c \
  -lole32 -luuid -lkernel32 -luser32 -lpsapi
ls -la out/winmm.dll
