@echo off
rem Builds out\winmm.dll on Windows. Needs zig from ziglang.org (or "pip install ziglang").
cd /d "%~dp0"
if not exist out mkdir out
set ZIG=zig
where zig >nul 2>nul || set ZIG=python -m ziglang
%ZIG% cc -target x86_64-windows-gnu -shared -O2 -s -o out\winmm.dll ^
  -Ithird_party\minhook\include src\main.c src\winmm_stubs.c src\census_stubs.c ^
  third_party\minhook\src\hook.c third_party\minhook\src\buffer.c third_party\minhook\src\trampoline.c third_party\minhook\src\hde\hde64.c ^
  -lole32 -luuid -lkernel32 -luser32 -lpsapi
if errorlevel 1 exit /b 1
%ZIG% cc -O2 -o out\mkini.exe tools\mkini.c
if errorlevel 1 exit /b 1
out\mkini.exe
echo Built out\winmm.dll, out\GearsHaptics.ini and out\GearsHaptics_events.ini
