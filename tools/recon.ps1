# Phase 0 recon for the E-Day haptics project.
# Read only: it changes nothing. Writes recon.txt next to this script.
# Run:  powershell -ExecutionPolicy Bypass -File .\recon.ps1
# Defaults to the Steam install at C:\Program Files (x86)\Steam\steamapps\common\GoWEDay (exe: FairlightConcept\Binaries\Win64\GoWEDay-Steam.exe). Override with -GameDir.

param([string]$GameDir = "C:\Program Files (x86)\Steam\steamapps\common\GoWEDay")

$ErrorActionPreference = "SilentlyContinue"
$ver = "recon 0.1"
$out = Join-Path $PSScriptRoot "recon.txt"
$log = New-Object System.Collections.Generic.List[string]
function W($s) { $log.Add([string]$s); Write-Host $s }

W "== $ver  $(Get-Date -Format s)"

# ---------- find the game ----------
if (-not $GameDir) {
    $steam = (Get-ItemProperty "HKCU:\Software\Valve\Steam").SteamPath
    $libs = @()
    if ($steam) {
        $libs += $steam
        $vdf = Join-Path $steam "steamapps\libraryfolders.vdf"
        if (Test-Path $vdf) {
            Select-String -Path $vdf -Pattern '"path"\s+"([^"]+)"' | ForEach-Object {
                $libs += ($_.Matches[0].Groups[1].Value -replace '\\\\','\')
            }
        }
    }
    foreach ($l in ($libs | Select-Object -Unique)) {
        $acf = Join-Path $l "steamapps\appmanifest_3010850.acf"
        if (Test-Path $acf) {
            $dir = (Select-String -Path $acf -Pattern '"installdir"\s+"([^"]+)"').Matches[0].Groups[1].Value
            $GameDir = Join-Path $l "steamapps\common\$dir"
            break
        }
    }
}
if (-not $GameDir -or -not (Test-Path $GameDir)) {
    W "GAME NOT FOUND. Re-run with -GameDir ""<path to the game folder>"""
    $log | Set-Content $out -Encoding UTF8; exit 1
}
W "GameDir: $GameDir"

# ---------- top level + binaries ----------
W "`n== Top level"
Get-ChildItem $GameDir | ForEach-Object { W ("  {0}{1}" -f $_.Name, $(if ($_.PSIsContainer) {"\"} else {"  ($([math]::Round($_.Length/1MB,1)) MB)"})) }

$exes = Get-ChildItem $GameDir -Recurse -Filter *.exe | Where-Object { $_.FullName -match 'Binaries\\Win64' }
W "`n== Win64 exes"
$exes | ForEach-Object { W ("  {0}  ({1} MB)" -f $_.FullName.Substring($GameDir.Length), [math]::Round($_.Length/1MB,1)) }
$main = $exes | Where-Object { $_.Name -eq 'GoWEDay-Steam.exe' } | Select-Object -First 1
if (-not $main) { $main = $exes | Sort-Object Length -Descending | Select-Object -First 1 }
if (-not $main) { W "No Win64 exe found"; $log | Set-Content $out -Encoding UTF8; exit 1 }
$binDir = $main.DirectoryName
W "Main exe: $($main.Name)  version: $((Get-Item $main.FullName).VersionInfo.FileVersion)"

W "`n== DLLs next to main exe"
Get-ChildItem $binDir -Filter *.dll | ForEach-Object { W ("  {0}  ({1} KB)" -f $_.Name, [math]::Round($_.Length/1KB)) }

# ---------- anti-cheat / middleware markers on disk ----------
W "`n== Markers on disk"
$markers = 'EasyAntiCheat','BattlEye','EOSSDK','libScePad','ScePad','Wwise','AkSoundEngine','fmod','GameInput','SDL','xinput','hid'
Get-ChildItem $GameDir -Recurse -File | Where-Object {
    $n = $_.Name; ($markers | Where-Object { $n -match $_ }).Count -gt 0
} | Select-Object -First 80 | ForEach-Object { W ("  " + $_.FullName.Substring($GameDir.Length)) }
W ("  .bnk count: " + (Get-ChildItem $GameDir -Recurse -Filter *.bnk).Count)
W ("  .wem count: " + (Get-ChildItem $GameDir -Recurse -Filter *.wem).Count)

# ---------- PE imports / exports (no external tools) ----------
function Read-PE($path) {
    $b = [IO.File]::ReadAllBytes($path)
    $pe = [BitConverter]::ToInt32($b, 0x3C)
    $nsec = [BitConverter]::ToUInt16($b, $pe + 6)
    $optSize = [BitConverter]::ToUInt16($b, $pe + 20)
    $opt = $pe + 24
    $dd = $opt + 112                       # PE32+ data directories
    $secs = @()
    $s = $opt + $optSize
    for ($i = 0; $i -lt $nsec; $i++) {
        $o = $s + $i * 40
        $secs += [pscustomobject]@{
            Name = [Text.Encoding]::ASCII.GetString($b, $o, 8).Trim([char]0)
            VA = [BitConverter]::ToUInt32($b, $o + 12); VS = [BitConverter]::ToUInt32($b, $o + 8)
            Raw = [BitConverter]::ToUInt32($b, $o + 20); RS = [BitConverter]::ToUInt32($b, $o + 16) }
    }
    $rva2off = { param($r) foreach ($x in $secs) { if ($r -ge $x.VA -and $r -lt $x.VA + [math]::Max($x.VS,$x.RS)) { return $r - $x.VA + $x.Raw } }; return -1 }
    $cstr = { param($o) $e = $o; while ($e -lt $b.Length -and $b[$e] -ne 0) { $e++ }; [Text.Encoding]::ASCII.GetString($b, $o, $e - $o) }
    $res = [pscustomobject]@{ Bytes = $b; Sections = $secs; Exports = @(); Imports = @{} }
    # exports
    $erva = [BitConverter]::ToUInt32($b, $dd)
    if ($erva) {
        $eo = & $rva2off $erva
        $nn = [BitConverter]::ToUInt32($b, $eo + 24)
        $no = & $rva2off ([BitConverter]::ToUInt32($b, $eo + 32))
        for ($i = 0; $i -lt $nn; $i++) { $res.Exports += & $cstr (& $rva2off ([BitConverter]::ToUInt32($b, $no + 4*$i))) }
    }
    # imports
    $irva = [BitConverter]::ToUInt32($b, $dd + 8)
    if ($irva) {
        $io = & $rva2off $irva
        while ($true) {
            $oft = [BitConverter]::ToUInt32($b, $io); $nameR = [BitConverter]::ToUInt32($b, $io + 12); $ft = [BitConverter]::ToUInt32($b, $io + 16)
            if ($nameR -eq 0) { break }
            $dll = & $cstr (& $rva2off $nameR)
            $t = & $rva2off ($(if ($oft) {$oft} else {$ft}))
            $fns = @()
            while ($true) {
                $v = [BitConverter]::ToUInt64($b, $t); if ($v -eq 0) { break }
                if ($v -band 0x8000000000000000) { $fns += "#" + ($v -band 0xFFFF) } else { $fns += & $cstr ((& $rva2off ([uint32]$v)) + 2) }
                $t += 8
            }
            $res.Imports[$dll] = $fns
            $io += 20
        }
    }
    return $res
}

W "`n== Main exe PE"
$pe = Read-PE $main.FullName
W "Sections: $(($pe.Sections | ForEach-Object { $_.Name }) -join ', ')"
W "Export count: $($pe.Exports.Count)"
$interesting = 'Ak|Wwise|fmod|scePad|ScePad|XInput|HidD_|SetupDi|WriteFile|DeviceIoControl|GameInput|SDL|Audio|IMMDevice|CoCreateInstance'
W "Interesting exports (first 60):"
$pe.Exports | Where-Object { $_ -match $interesting } | Select-Object -First 60 | ForEach-Object { W "  $_" }
W "Imported DLLs:"
foreach ($k in ($pe.Imports.Keys | Sort-Object)) {
    $hit = $pe.Imports[$k] | Where-Object { $_ -match $interesting }
    W ("  {0} ({1} fns){2}" -f $k, $pe.Imports[$k].Count, $(if ($hit) { "  -> " + ($hit -join ', ') } else { "" }))
}

# ---------- strings in the exe ----------
W "`n== String markers in main exe"
$txt = [Text.Encoding]::GetEncoding(28591).GetString($pe.Bytes)
$probe = 'AkSoundEngine','AK::SoundEngine','Wwise','FMOD','fmod_studio','libScePad','scePadSetVibration','scePadSetTriggerEffect','scePadOpen','GameInput','XInputSetState','SDL_GameControllerRumble','SDL_JoystickRumble','DualSense','EasyAntiCheat','BattlEye','HidD_SetOutputReport','IAudioClient','haptic','Haptic','ForceFeedback'
foreach ($p in $probe) { $c = ([regex]::Matches($txt, [regex]::Escape($p))).Count; W ("  {0,-28} {1}" -f $p, $c) }
$pe = $null; $txt = $null; [GC]::Collect()

# ---------- other DLLs: exports of interest ----------
W "`n== Exports of interest in other DLLs"
Get-ChildItem $GameDir -Recurse -Filter *.dll | Where-Object { $_.Length -lt 200MB } | ForEach-Object {
    $p = Read-PE $_.FullName
    $hit = $p.Exports | Where-Object { $_ -match $interesting }
    if ($hit) { W ("  {0}: {1} hits, e.g. {2}" -f $_.FullName.Substring($GameDir.Length), $hit.Count, (($hit | Select-Object -First 8) -join ', ')) }
}

# ---------- controller side ----------
W "`n== DualSense audio endpoint"
Get-PnpDevice -Class AudioEndpoint | Where-Object { $_.FriendlyName -match 'Wireless Controller|DualSense' } | ForEach-Object { W ("  {0}  [{1}]" -f $_.FriendlyName, $_.Status) }
W "`n== Steam Input setting hint: make sure Steam Input is OFF for this game (Properties > Controller)."

$log | Set-Content $out -Encoding UTF8
W "`nSaved: $out"
