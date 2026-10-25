/*
 * Gears Haptics: Gears of War: E-Day DualSense haptics. winmm.dll proxy (formerly lantern, briefly Lambent).
 * Phase 1: catch the game's DualSense rumble in its HID output reports,
 * strip the motor values (triggers, lights untouched) and play real
 * waveforms on the voice coils through the controller's USB audio endpoint.
 */
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#define INITGUID
#include <windows.h>
#include <winioctl.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <endpointvolume.h>
#include <cfgmgr32.h>
#include <devpkey.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "MinHook.h"
#include "winmm_names.h"
#include "events_default.h"

#define LANTERN_VERSION "0.2.4"

/* ---------------- winmm forwarding ---------------- */
void *winmm_ptrs[WINMM_COUNT];
static HMODULE g_realWinmm;

static void load_real_winmm(void)
{
    wchar_t path[MAX_PATH];
    GetSystemDirectoryW(path, MAX_PATH);
    wcscat(path, L"\\winmm.dll");
    g_realWinmm = LoadLibraryW(path);
    for (int i = 0; i < WINMM_COUNT; i++)
        winmm_ptrs[i] = g_realWinmm ? (void *)GetProcAddress(g_realWinmm, winmm_names[i]) : NULL;
}

/* ---------------- log + config ---------------- */
static wchar_t g_dir[MAX_PATH];
static FILE *g_log;
static CRITICAL_SECTION g_logCs;

static void L(const char *fmt, ...)
{
    if (!g_log) return;
    SYSTEMTIME t; GetLocalTime(&t);
    EnterCriticalSection(&g_logCs);
    fprintf(g_log, "[%02d:%02d:%02d.%03d] ", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
    va_list a; va_start(a, fmt); vfprintf(g_log, fmt, a); va_end(a);
    fputc('\n', g_log); fflush(g_log);
    LeaveCriticalSection(&g_logCs);
}

static struct {
    int enabled, keepNative, logReports, hookWwise, logPosts, nameScan, census, audioTap;
    float curve, evGain, cineHold; int cineMute;
    float strength, lowHz, highHz, lowGain, highGain, bleed, trGain, trThresh, chime;
} C;

static wchar_t g_ini[MAX_PATH];
static float ini_f(const wchar_t *sec, const wchar_t *key, float def)
{
    wchar_t buf[64], d[64];
    swprintf(d, 64, L"%g", def);
    GetPrivateProfileStringW(sec, key, d, buf, 64, g_ini);
    return (float)_wtof(buf);
}

#include "defaults_ini.h"

/* ---------------- sound event haptics (allowlist) ---------------- */
typedef struct { const char *name; float hz, decayMs, lenMs; } Preset;
static Preset g_presets[] = {
    { "tick", 230, 10, 25 }, { "click", 160, 18, 45 }, { "knock", 110, 32, 80 },
    { "thump", 65, 70, 180 }, { "buzz", 185, 90, 140 },
};
#define NPRESET (int)(sizeof g_presets / sizeof g_presets[0])
#define MAXEV 2048
static struct { DWORD id; int preset; float gain; int side; int cat; } g_ev[MAXEV];
static const wchar_t *g_catNames[] = { L"Boots", L"Guns", L"Reload", L"Melee", L"Grenade", L"Chainsaw", L"Body", L"Damage", L"Pickups", L"World", L"HUD", L"Menus" };
#define NCAT (int)(sizeof g_catNames / sizeof g_catNames[0])
#define CAT_MENUS (NCAT - 1)
#define PRESET_CINE 99
static volatile ULONGLONG g_cineUntil;

/* sustained effects: a start event begins a pulse train that runs until a stop event,
   until the game's rumble goes quiet (stop=rumble), or until maxSec */
#define MAXLOOP 128
#define MAXSTOP 4
typedef struct { DWORD start; int preset; float gain; int side; int cat; float periodMs, maxSec, dubMs; int untilRumble; DWORD stop[MAXSTOP]; int nstop; int group; } LoopDef;
static LoopDef g_lp[MAXLOOP];
static int g_lpN;
/* one running state per group (lines with the same name share it) */
static volatile LONG g_lpOn[MAXLOOP];          /* def index + 1 currently driving the group, 0 = off */
static volatile ULONGLONG g_lpT0[MAXLOOP];
static wchar_t g_lpGroupName[MAXLOOP][24];
static int g_lpGroups;
static int g_evN;
static volatile LONG g_evQ[64], g_evW, g_evFired;

static void load_events(void)
{
    static wchar_t buf[262144];
    wchar_t evp[MAX_PATH]; wcscpy(evp, g_dir); wcscat(evp, L"GearsHaptics_events.ini");
    {   /* replace an events file from an older version (keeps it as GearsHaptics_events.old.ini) */
        HANDLE h = CreateFileW(evp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            char hd[512] = ""; DWORD r = 0; ReadFile(h, hd, sizeof hd - 1, &r, NULL); hd[r] = 0; CloseHandle(h);
            if (!strstr(hd, "format 3")) {
                wchar_t old[MAX_PATH]; wcscpy(old, g_dir); wcscat(old, L"GearsHaptics_events.old.ini");
                MoveFileExW(evp, old, MOVEFILE_REPLACE_EXISTING);
                L("[ev] old GearsHaptics_events.ini kept as GearsHaptics_events.old.ini, writing the new defaults");
            }
        }
    }
    if (GetFileAttributesW(evp) == INVALID_FILE_ATTRIBUTES) {
        HANDLE h = CreateFileW(evp, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, DEFAULT_EVENTS_INI, (DWORD)strlen(DEFAULT_EVENTS_INI), &w, NULL); CloseHandle(h); }
    }
    DWORD n = GetPrivateProfileSectionW(L"Events", buf, 262144, evp);
    for (wchar_t *e = buf; n && *e && g_evN < MAXEV; e += wcslen(e) + 1) {
        if (*e == L';') continue;
        DWORD id = (DWORD)wcstoul(e, NULL, 10);
        wchar_t *v = wcschr(e, L'='); if (!id || !v) continue;
        v++;
        wchar_t pn[16] = L""; int k = 0;
        while (*v && *v != L',' && k < 15) pn[k++] = (wchar_t)towlower(*v++);
        pn[k] = 0;
        int pi = -1;
        if (!wcscmp(pn, L"cine")) pi = PRESET_CINE;
        for (int i = 0; i < NPRESET; i++) { wchar_t w[16]; swprintf(w, 16, L"%hs", g_presets[i].name); if (!wcscmp(w, pn)) pi = i; }
        if (pi < 0) { L("[ev] unknown preset '%ls' for %u", pn, id); continue; }
        float gain = 1.f; int side = 0;
        if (*v == L',') { v++; gain = (float)wcstod(v, &v); }
        if (*v == L',') { v++; side = (*v == L'L' || *v == L'l') ? 1 : (*v == L'R' || *v == L'r') ? 2 : (*v == L'A' || *v == L'a') ? 3 : 0; while (*v && *v != L',') v++; }
        int cat = -1;
        if (*v == L',') {
            v++; wchar_t cn[24]; int q = 0;
            while (*v && *v != L' ' && *v != L';' && *v != L',' && q < 23) cn[q++] = *v++;
            cn[q] = 0;
            for (int c = 0; c < NCAT; c++) if (!_wcsicmp(cn, g_catNames[c])) cat = c;
        }
        if (cat >= 0) { float cg = ini_f(L"Sound", g_catNames[cat], cat == 0 ? 0.6f : 1.0f); if (cg < 0) cg = 0; if (cg > 2) cg = 2; gain *= cg; }
        if (pi != PRESET_CINE && gain <= 0) continue;
        g_ev[g_evN].id = id; g_ev[g_evN].preset = pi; g_ev[g_evN].gain = gain; g_ev[g_evN].side = side; g_ev[g_evN].cat = cat; g_evN++;
    }
    n = GetPrivateProfileSectionW(L"Loops", buf, 262144, evp);
    for (wchar_t *e = buf; n && *e && g_lpN < MAXLOOP; e += wcslen(e) + 1) {
        /* startId=group,preset,gain,side,category,periodMs,maxSec,dubMs,stop  (stop = rumble or ids joined by |) */
        if (*e == L';') continue;
        DWORD id = (DWORD)wcstoul(e, NULL, 10);
        wchar_t *v = wcschr(e, L'='); if (!id || !v) continue;
        wchar_t f[9][48]; int nf = 0; v++;
        while (nf < 9) {
            int q = 0; while (*v && *v != L',' && *v != L';' && q < 47) f[nf][q++] = *v++;
            while (q && f[nf][q - 1] == L' ') q--;
            f[nf][q] = 0; nf++;
            if (*v != L',') break; v++;
        }
        if (nf < 9) { L("[loop] bad line for %u", id); continue; }
        LoopDef *d = &g_lp[g_lpN]; memset(d, 0, sizeof *d);
        d->start = id; d->preset = -1;
        for (int i = 0; i < NPRESET; i++) { wchar_t w[16]; swprintf(w, 16, L"%hs", g_presets[i].name); if (!_wcsicmp(w, f[1])) d->preset = i; }
        if (d->preset < 0) { L("[loop] unknown preset for %u", id); continue; }
        d->gain = (float)_wtof(f[2]);
        d->side = (f[3][0] == L'L') ? 1 : (f[3][0] == L'R') ? 2 : (f[3][0] == L'A') ? 3 : 0;
        d->cat = -1; for (int c = 0; c < NCAT; c++) if (!_wcsicmp(f[4], g_catNames[c])) d->cat = c;
        if (d->cat >= 0) { float cg = ini_f(L"Sound", g_catNames[d->cat], d->cat == 0 ? 0.6f : 1.0f); if (cg < 0) cg = 0; if (cg > 2) cg = 2; d->gain *= cg; }
        d->periodMs = (float)_wtof(f[5]); d->maxSec = (float)_wtof(f[6]); d->dubMs = (float)_wtof(f[7]);
        if (d->periodMs < 20) d->periodMs = 20; if (d->maxSec <= 0) d->maxSec = 5;
        wchar_t *sp = f[8];
        while (*sp) {
            while (*sp == L'|' || *sp == L' ') sp++;
            if (!*sp) break;
            if (!_wcsnicmp(sp, L"rumble", 6)) { d->untilRumble = 1; sp += 6; continue; }
            wchar_t *en; DWORD sid = (DWORD)wcstoul(sp, &en, 10);
            if (en == sp) break;
            if (d->nstop < MAXSTOP) d->stop[d->nstop++] = sid;
            sp = en;
        }
        int g = -1; for (int i = 0; i < g_lpGroups; i++) if (!_wcsicmp(g_lpGroupName[i], f[0])) g = i;
        if (g < 0) { g = g_lpGroups++; wcsncpy(g_lpGroupName[g], f[0], 23); }
        d->group = g;
        if (d->gain <= 0) continue;
        g_lpN++;
    }
    L("[ev] %d allowlisted sound events, %d loop starts in %d groups", g_evN, g_lpN, g_lpGroups);
}

/* called on the game's thread from the PostEvent hook: queue only, never block */
static void ev_post(DWORD id)
{
    for (int i = 0; i < g_lpN; i++) {
        LoopDef *d = &g_lp[i];
        if (d->start == id) {
            if (!(C.cineMute && GetTickCount64() < g_cineUntil)) { g_lpT0[d->group] = GetTickCount64(); InterlockedExchange(&g_lpOn[d->group], i + 1); }
        }
        for (int k = 0; k < d->nstop; k++) if (d->stop[k] == id && g_lpOn[d->group] == i + 1) InterlockedExchange(&g_lpOn[d->group], 0);
    }
    for (int i = 0; i < g_evN; i++) if (g_ev[i].id == id) {
        if (g_ev[i].preset == PRESET_CINE) { if (C.cineMute) g_cineUntil = GetTickCount64() + (ULONGLONG)(C.cineHold * 1000); return; }
        if (C.cineMute && g_ev[i].cat != CAT_MENUS && GetTickCount64() < g_cineUntil) return;
        LONG w = InterlockedIncrement(&g_evW) - 1;
        InterlockedExchange(&g_evQ[w & 63], i + 1);
        InterlockedIncrement(&g_evFired);
        return;
    }
}

static void load_config(void)
{
    if (GetFileAttributesW(g_ini) == INVALID_FILE_ATTRIBUTES) {
        HANDLE h = CreateFileW(g_ini, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, DEFAULT_INI, (DWORD)strlen(DEFAULT_INI), &w, NULL); CloseHandle(h); }
    }
    {
        wchar_t t[64];
        if (!GetPrivateProfileSectionW(L"Sound", t, 64, g_ini)) {
            HANDLE h = CreateFileW(g_ini, FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, "\r\n", 2, &w, NULL); WriteFile(h, DEFAULT_SOUND_INI, (DWORD)strlen(DEFAULT_SOUND_INI), &w, NULL); CloseHandle(h); }
        }
    }
    C.enabled    = (int)ini_f(L"General", L"Enabled", 1);
    C.strength   = ini_f(L"General", L"Strength", 1.0f);
    C.keepNative = (int)ini_f(L"General", L"KeepNativeRumble", 0);
    C.logReports = (int)ini_f(L"General", L"LogReports", 200);
    C.hookWwise = (int)ini_f(L"Diag", L"HookWwise", 1);
    C.logPosts   = (int)ini_f(L"Diag", L"LogPosts", 0);
    C.nameScan   = (int)ini_f(L"Diag", L"NameScan", 0);
    C.census     = (int)ini_f(L"Diag", L"GameInputCensus", 0);
    C.audioTap   = (int)ini_f(L"Diag", L"AudioTap", 0);
    C.curve      = ini_f(L"Tuning", L"Curve", 0.5f);
    C.evGain     = ini_f(L"Tuning", L"EventGain", 1.0f);
    C.cineMute   = (int)ini_f(L"Sound", L"CutsceneMute", 1);
    C.cineHold   = ini_f(L"Sound", L"CutsceneHoldSec", 8);
    for (int i = 0; i < NPRESET; i++) {
        wchar_t key[16], d[64], b[64]; swprintf(key, 16, L"%hs", g_presets[i].name);
        swprintf(d, 64, L"%g,%g,%g", g_presets[i].hz, g_presets[i].decayMs, g_presets[i].lenMs);
        GetPrivateProfileStringW(L"Presets", key, d, b, 64, g_ini);
        float hz, de, le;
        if (swscanf(b, L"%f,%f,%f", &hz, &de, &le) == 3 && hz >= 20 && hz <= 1000 && de > 0 && le > 0 && le <= 2000) {
            g_presets[i].hz = hz; g_presets[i].decayMs = de; g_presets[i].lenMs = le;
        }
    }
    if (C.curve < 0.1f) C.curve = 0.1f; if (C.curve > 2.f) C.curve = 2.f;
    C.chime      = ini_f(L"General", L"Chime", 1);
    C.lowHz      = ini_f(L"Tuning", L"LowMotorHz", 75);
    C.highHz     = ini_f(L"Tuning", L"HighMotorHz", 185);
    C.lowGain    = ini_f(L"Tuning", L"LowGain", 1.0f);
    C.highGain   = ini_f(L"Tuning", L"HighGain", 0.8f);
    C.bleed      = ini_f(L"Tuning", L"SideBleed", 0.6f);
    C.trGain     = ini_f(L"Tuning", L"TransientGain", 1.0f);
    C.trThresh   = ini_f(L"Tuning", L"TransientThreshold", 0.04f);
    L("config: Enabled=%d Strength=%.2f KeepNativeRumble=%d LowHz=%.0f HighHz=%.0f LowGain=%.2f HighGain=%.2f Bleed=%.2f TrGain=%.2f TrThr=%.2f Curve=%.2f EventGain=%.2f LogPosts=%d NameScan=%d",
      C.enabled, C.strength, C.keepNative, C.lowHz, C.highHz, C.lowGain, C.highGain, C.bleed, C.trGain, C.trThresh, C.curve, C.evGain, C.logPosts, C.nameScan);
    load_events();
}

/* ---------------- shared state ---------------- */
static volatile LONG g_hapticsLive;      /* audio endpoint streaming */
static volatile float g_low, g_high;     /* 0..1 from the game's rumble */
static volatile LONG g_chimeReq;

/* stats */
static volatile LONG s_writes, s_ioctl, s_rumble, s_stripped, s_bt, s_logged;
static volatile LONG s_maxLow, s_maxHigh;

/* ---------------- DualSense handle tracking ---------------- */
#define MAXH 64
static struct { HANDLE h; int ds; } g_h[MAXH];
static SRWLOCK g_hLock = SRWLOCK_INIT;
static __thread int t_inHook;

static BOOLEAN (WINAPI *pHidD_GetAttributes)(HANDLE, void *);
typedef struct { ULONG Size; USHORT VendorID, ProductID, VersionNumber; } HIDATTR;

static int is_ds_path_w(const wchar_t *p)
{
    if (!p) return 0;
    wchar_t low[512]; int i = 0;
    for (; p[i] && i < 511; i++) low[i] = towlower(p[i]);
    low[i] = 0;
    return wcsstr(low, L"vid_054c") && (wcsstr(low, L"pid_0ce6") || wcsstr(low, L"pid_0df2"));
}

static void remember(HANDLE h, int ds)
{
    AcquireSRWLockExclusive(&g_hLock);
    int slot = -1;
    for (int i = 0; i < MAXH; i++) {
        if (g_h[i].h == h) { slot = i; break; }
        if (slot < 0 && g_h[i].h == NULL) slot = i;
    }
    if (slot < 0 && ds) slot = (int)(((uintptr_t)h >> 2) % MAXH);
    if (slot >= 0) { g_h[slot].h = h; g_h[slot].ds = ds; }
    ReleaseSRWLockExclusive(&g_hLock);
}

/* 1 = DualSense, 0 = not, -1 = unknown */
static int lookup(HANDLE h)
{
    int r = -1;
    AcquireSRWLockShared(&g_hLock);
    for (int i = 0; i < MAXH; i++) if (g_h[i].h == h) { r = g_h[i].ds; break; }
    ReleaseSRWLockShared(&g_hLock);
    return r;
}

static int handle_is_ds(HANDLE h)
{
    int r = lookup(h);
    if (r >= 0) return r;
    r = 0;
    if (pHidD_GetAttributes) {
        HIDATTR a = { sizeof(a) };
        t_inHook++;
        if (pHidD_GetAttributes(h, &a) && a.VendorID == 0x054C && (a.ProductID == 0x0CE6 || a.ProductID == 0x0DF2)) r = 1;
        t_inHook--;
    }
    remember(h, r);
    if (r) L("[hid] DualSense handle %p found by attributes (opened before hooks)", h);
    return r;
}

static void hexlog(const char *tag, const BYTE *b, DWORD n)
{
    char s[3 * 64 + 1]; int k = 0;
    for (DWORD i = 0; i < n && i < 64; i++) k += sprintf(s + k, "%02X ", b[i]);
    s[k] = 0;
    L("%s len=%lu: %s", tag, n, s);
}

static void max_store(volatile LONG *m, LONG v) { LONG o; while ((o = *m) < v && InterlockedCompareExchange(m, v, o) != o) {} }

/* b is the game's own buffer; motor bytes are edited in place. */
static BYTE g_lastRep[64];
static void process_report(BYTE *b, DWORD n, const char *via)
{
    DWORD cn = n > 64 ? 64 : n;
    if (s_logged < C.logReports && memcmp(g_lastRep, b, cn) != 0) { InterlockedIncrement(&s_logged); memcpy(g_lastRep, b, cn); hexlog(via, b, n); }
    if (n < 1) return;
    if (b[0] == 0x31) {                         /* Bluetooth report, haptics need USB */
        if (InterlockedIncrement(&s_bt) == 1) L("[hid] Bluetooth output reports seen. Haptics only work over USB: unplug Bluetooth, use the cable.");
        return;
    }
    if (b[0] != 0x02 || n < 48) return;
    BYTE f0 = b[1], f2 = b[39];
    /* 0.1.5: rumble is valid when flag0 bit0/1 (old firmware) OR flag2 (byte 39) bit 0x04,
     * COMPATIBLE_VIBRATION2 on newer firmware. E-Day uses the second one. */
    if ((f0 & 0x03) || (f2 & 0x04)) {
        BYTE hi = b[3], lo = b[4];             /* [3] right/light motor, [4] left/heavy motor */
        static BYTE lastHi, lastLo; static volatile LONG motorLogs;
        if ((hi != lastHi || lo != lastLo) && motorLogs < 400) {
            InterlockedIncrement(&motorLogs);
            L("[motor] low=%u high=%u (f0=%02X f2=%02X)", lo, hi, f0, f2);
        }
        lastHi = hi; lastLo = lo;
        g_high = hi / 255.0f; g_low = lo / 255.0f;
        if (hi || lo) { InterlockedIncrement(&s_rumble); max_store(&s_maxLow, lo); max_store(&s_maxHigh, hi); }
        if (C.enabled && g_hapticsLive && !C.keepNative) {
            b[1] = (BYTE)(f0 & ~0x03);          /* drop rumble emulation, keep trigger/light flags */
            b[39] = (BYTE)(f2 & ~0x04);
            b[3] = 0; b[4] = 0;
            InterlockedIncrement(&s_stripped);
        }
    }
}

/* ---------------- GameInput rumble ----------------
 * 0.1.1 showed the game's rumble is neither in its HID reports nor an audio
 * stream. UE 5.6 drives pads through GameInput, whose service writes the
 * rumble from another process. Catch IGameInputDevice::SetRumbleState here.
 * Vtable indexes from the public GameInput.h (NuGet Microsoft.GameInput 4.0.302). */
typedef struct { float lowFrequency, highFrequency, leftTrigger, rightTrigger; } GIRumble;
typedef void (STDMETHODCALLTYPE *SetRumbleFn)(void *, const GIRumble *);
typedef HRESULT (STDMETHODCALLTYPE *SendRawFn)(void *, void *);

typedef struct { const char *name; GUID gi; int legacyQI; int idxCurReading, idxGetDevice, idxRumble, idxSendRaw; int kindGamepadArg3; } GIVer;
/* v0 legacy object from GameInputCreate; v1/v2 via QueryInterface on it; v3/v4 via GameInputInitialize */
static GIVer g_giv[5] = {
    { "v0", {0x11BE2A7E,0x4254,0x445A,{0x9C,0x09,0xFF,0xC4,0x0F,0x00,0x69,0x18}}, 0, 4, 6, 10, 17, 1 },
    { "v1", {0x40FFB7E4,0x6150,0x407A,{0xB4,0x39,0x13,0x2B,0xAD,0xC0,0x8D,0x2D}}, 1, 4, 5, 8, -1, 1 },
    { "v2", {0xBBAA66D2,0x837A,0x40F7,{0xA3,0x03,0x91,0x7D,0x50,0x09,0x55,0xF4}}, 1, 4, 5, 9, -1, 1 },
    { "v3", {0x20EFC1C7,0x5D9A,0x43BA,{0xB2,0x6F,0xB8,0x07,0xFA,0x48,0x60,0x9C}}, 0, 4, 5, 9, 17, 1 },
    { "v4", {0xF2DAAE4C,0x4D21,0x4250,{0x89,0x53,0x4F,0x79,0x1E,0x62,0x79,0xAB}}, 0, 4, 5, 47, 57, 1 },
};
static volatile LONG s_giRumble, s_giRumbleNZ, s_giRaw;
static volatile LONG g_giHooked[5];
static HRESULT (WINAPI *pGICreate)(void **);
static HRESULT (WINAPI *pGIInit)(REFIID, void **);
static HRESULT (WINAPI *oGICreate)(void **);
static HRESULT (WINAPI *oGIInit)(REFIID, void **);
static volatile LONG g_giSeen;

static void gi_rumble(int v, const GIRumble *p, SetRumbleFn orig, void *self)
{
    LONG n = InterlockedIncrement(&s_giRumble);
    float lo = p ? p->lowFrequency : 0.f, hi = p ? p->highFrequency : 0.f;
    if (lo > 1) lo = 1; if (hi > 1) hi = 1; if (lo < 0) lo = 0; if (hi < 0) hi = 0;
    g_low = lo; g_high = hi;
    if (lo > 0 || hi > 0) { InterlockedIncrement(&s_giRumbleNZ); max_store(&s_maxLow, (LONG)(lo * 255)); max_store(&s_maxHigh, (LONG)(hi * 255)); }
    if (n <= 40 || (n % 500) == 0)
        L("[gi] %s SetRumbleState #%ld low=%.3f high=%.3f lt=%.3f rt=%.3f", g_giv[v].name, n, lo, hi, p ? p->leftTrigger : 0.f, p ? p->rightTrigger : 0.f);
    if (C.enabled && g_hapticsLive && !C.keepNative) {
        GIRumble z = {0};
        if (p) { z.leftTrigger = p->leftTrigger; z.rightTrigger = p->rightTrigger; }
        InterlockedIncrement(&s_stripped);
        orig(self, &z);
    } else orig(self, p);
}

#define GI_DETOURS(i) \
    static SetRumbleFn oRumble##i; static SendRawFn oRaw##i; \
    static void STDMETHODCALLTYPE hRumble##i(void *self, const GIRumble *p) { gi_rumble(i, p, oRumble##i, self); } \
    static HRESULT STDMETHODCALLTYPE hRaw##i(void *self, void *rep) { \
        if (InterlockedIncrement(&s_giRaw) <= 20) L("[gi] %s SendRawDeviceOutput report %p", g_giv[i].name, rep); \
        return oRaw##i(self, rep); }
GI_DETOURS(0) GI_DETOURS(1) GI_DETOURS(2) GI_DETOURS(3) GI_DETOURS(4)
static void *g_hRumble[5] = { hRumble0, hRumble1, hRumble2, hRumble3, hRumble4 };
static SetRumbleFn *g_oRumble[5] = { &oRumble0, &oRumble1, &oRumble2, &oRumble3, &oRumble4 };
static void *g_hRaw[5] = { hRaw0, hRaw1, hRaw2, hRaw3, hRaw4 };
static SendRawFn *g_oRaw[5] = { &oRaw0, &oRaw1, &oRaw2, &oRaw3, &oRaw4 };
static void *g_rumbleTargets[5];

static HRESULT WINAPI hGICreate(void **out)
{
    HRESULT hr = oGICreate(out);
    if (InterlockedIncrement(&g_giSeen) <= 5) L("[gi] game called GameInputCreate -> 0x%08lX", hr);
    return hr;
}
static HRESULT WINAPI hGIInit(REFIID riid, void **out)
{
    HRESULT hr = oGIInit(riid, out);
    const char *nm = "unknown IID";
    for (int i = 0; i < 5; i++) if (IsEqualGUID(riid, &g_giv[i].gi)) nm = g_giv[i].name;
    if (InterlockedIncrement(&g_giSeen) <= 5)
        L("[gi] game called GameInputInitialize(%s {%08lX-...}) -> 0x%08lX", nm, riid ? riid->Data1 : 0, hr);
    return hr;
}

static void gi_hook_module(HMODULE m, const wchar_t *nm)
{
    /* 0.1.2-p1 bug: a datafile/resource mapping of GameInput.dll latched this and hid the real load.
       Only latch once the exports resolve. */
    static volatile LONG done, logged;
    if (done) return;
    wchar_t path[MAX_PATH] = L""; GetModuleFileNameW(m, path, MAX_PATH);
    void *c = (void *)GetProcAddress(m, "GameInputCreate");
    void *i = (void *)GetProcAddress(m, "GameInputInitialize");
    if (InterlockedIncrement(&logged) <= 10)
        L("[gi] GameInput module seen: %ls (%ls) handle=%p create=%p init=%p", path, nm, m, c, i);
    if (!c && !i) return;
    if (InterlockedExchange(&done, 1)) return;
    pGICreate = c; pGIInit = i;
    if (pGICreate && MH_CreateHook(pGICreate, hGICreate, (void **)&oGICreate) == MH_OK) MH_EnableHook(pGICreate);
    if (pGIInit && MH_CreateHook(pGIInit, hGIInit, (void **)&oGIInit) == MH_OK) MH_EnableHook(pGIInit);
    L("[gi] hooked exports: GameInputCreate=%p GameInputInitialize=%p", pGICreate, pGIInit);
}

/* ---------------- XInput rumble (insurance) ----------------
 * If the game or a layer above it (Steam Input) drives rumble through XInput, catch it here. */
typedef struct { WORD l, r; } XVib;
typedef DWORD (WINAPI *XSetFn)(DWORD, XVib *);
static XSetFn oXSet[4]; static volatile LONG s_xi, s_xiNZ; static volatile LONG g_xiDone[4];
static DWORD xi_set(int k, DWORD u, XVib *v)
{
    LONG n = InterlockedIncrement(&s_xi);
    float lo = v ? v->l / 65535.f : 0.f, hi = v ? v->r / 65535.f : 0.f;
    if (lo > 0 || hi > 0) { InterlockedIncrement(&s_xiNZ); g_low = lo; g_high = hi; max_store(&s_maxLow, (LONG)(lo * 255)); max_store(&s_maxHigh, (LONG)(hi * 255)); }
    else if (n > 1) { g_low = 0; g_high = 0; }
    if (n <= 40 || (n % 500) == 0) L("[xi] XInputSetState #%ld user=%lu low=%.3f high=%.3f", n, u, lo, hi);
    if (C.enabled && g_hapticsLive && !C.keepNative && v) { XVib z = {0, 0}; return oXSet[k](u, &z); }
    return oXSet[k](u, v);
}
static DWORD WINAPI hX0(DWORD u, XVib *v) { return xi_set(0, u, v); }
static DWORD WINAPI hX1(DWORD u, XVib *v) { return xi_set(1, u, v); }
static DWORD WINAPI hX2(DWORD u, XVib *v) { return xi_set(2, u, v); }
static DWORD WINAPI hX3(DWORD u, XVib *v) { return xi_set(3, u, v); }
static void xi_scan(void)
{
    static const wchar_t *mods[4] = { L"xinput1_4.dll", L"xinput1_3.dll", L"xinput9_1_0.dll", L"xinputuap.dll" };
    static void *hk[4] = { hX0, hX1, hX2, hX3 };
    for (int k = 0; k < 4; k++) {
        if (g_xiDone[k]) continue;
        HMODULE m = GetModuleHandleW(mods[k]); if (!m) continue;
        void *f = (void *)GetProcAddress(m, "XInputSetState"); if (!f) continue;
        g_xiDone[k] = 1;
        MH_STATUS st = MH_CreateHook(f, hk[k], (void **)&oXSet[k]);
        if (st == MH_OK) st = MH_EnableHook(f);
        L("[xi] %ls loaded, hook XInputSetState @%p: %s", mods[k], f, MH_StatusToString(st));
    }
}

/* Build our own GameInput object per version, take a gamepad reading, get its device and hook its vtable. */
static void cen_table(int t, void **vt, const char *nm);
static void gi_probe(void)
{
    for (int v = 0; v < 5; v++) {
        if (v != 0 && v != 3) g_giHooked[v] = 1; /* census only the versions the game uses */
        if (g_giHooked[v]) continue;
        GIVer *V = &g_giv[v];
        void *gi = NULL; HRESULT hr;
        if (v <= 2) {
            if (!pGICreate) continue;
            void *legacy = NULL;
            hr = (oGICreate ? oGICreate : pGICreate)(&legacy);
            if (FAILED(hr) || !legacy) continue;
            if (v == 0) gi = legacy;
            else { IUnknown *u = legacy; if (FAILED(u->lpVtbl->QueryInterface(u, &V->gi, &gi))) gi = NULL; u->lpVtbl->Release(u); }
        } else {
            if (!pGIInit) continue;
            hr = (oGIInit ? oGIInit : pGIInit)(&V->gi, &gi);
            if (FAILED(hr)) gi = NULL;
        }
        if (!gi) { g_giHooked[v] = -1; continue; }
        void **vt = *(void ***)gi;
        typedef HRESULT (STDMETHODCALLTYPE *CurFn)(void *, int, void *, void **);
        void *rd = NULL;
        hr = ((CurFn)vt[V->idxCurReading])(gi, 0x00040000 /* GameInputKindGamepad */, NULL, &rd);
        if (SUCCEEDED(hr) && rd) {
            void **rvt = *(void ***)rd; void *dev = NULL;
            ((void (STDMETHODCALLTYPE *)(void *, void **))rvt[V->idxGetDevice])(rd, &dev);
            if (dev) {
                void **dvt = *(void ***)dev;
                cen_table(v == 0 ? 0 : 1, dvt, v == 0 ? "dev.v0" : "dev.v3");
                cen_table(v == 0 ? 2 : 3, *(void ***)gi, v == 0 ? "gi.v0" : "gi.v3");
                g_giHooked[v] = 1;
                ((IUnknown *)dev)->lpVtbl->Release((IUnknown *)dev);
            }
            ((IUnknown *)rd)->lpVtbl->Release((IUnknown *)rd);
        }
        ((IUnknown *)gi)->lpVtbl->Release((IUnknown *)gi);
    }
}


/* ---------------- 0.1.3 diagnostics: GameInput vtable census + Wwise PostEvent log ----------------
 * Log only. Stubs in census_stubs.S count calls and keep rdx without knowing the signature. */
#define CEN_N 256
extern void *cen_stub_table[CEN_N];
extern void *ak_stub_table[16];
void *g_cen_orig[CEN_N], *g_cen_rdx[CEN_N]; volatile LONG g_cen_cnt[CEN_N];
static LONG g_cen_last[CEN_N]; static void *g_cen_fn[CEN_N]; static const char *g_cen_name[4];
void *g_ak_orig[16]; static int g_ak_kind[16]; static int g_akN;

static int mod_range(void *addr, BYTE **base, SIZE_T *size)
{
    HMODULE m = NULL;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)addr, &m) || !m) return 0;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)((BYTE *)m + ((IMAGE_DOS_HEADER *)m)->e_lfanew);
    *base = (BYTE *)m; *size = nt->OptionalHeader.SizeOfImage; return 1;
}

static void cen_table(int t, void **vt, const char *nm)
{
    if (g_cen_name[t]) return;
    g_cen_name[t] = nm;
    BYTE *b; SIZE_T sz; int n = 0, alias = 0;
    if (!mod_range(vt[3], &b, &sz)) { L("[cen] %s: no module for slot 3", nm); return; }
    for (int sl = 3; sl < 64; sl++) {
        BYTE *f = vt[sl];
        if (f < b || f >= b + sz) break;
        int i = t * 64 + sl;
        MH_STATUS st;
        if (t == 0 && sl == 20) { /* raw output candidate: arg-preserving stub that calls raw_log(rdx) */
            st = MH_CreateHook(f, ak_stub_table[15], &g_ak_orig[15]);
            if (st == MH_OK) { MH_EnableHook(f); L("[raw] dev.v0 slot 20 @%p routed to dumper", f); }
            continue;
        }
        st = MH_CreateHook(f, cen_stub_table[i], &g_cen_orig[i]);
        if (st == MH_OK) { MH_EnableHook(f); g_cen_fn[i] = f; n++; }
        else if (st == MH_ERROR_ALREADY_CREATED) alias++;
        else L("[cen] %s slot %d @%p: %s", nm, sl, f, MH_StatusToString(st));
    }
    L("[cen] %s vtable %p: %d slots hooked, %d aliases (shared targets count once, under the first table)", nm, vt, n, alias);
}

static void cen_report(void)
{
    int lines = 0;
    for (int i = 0; i < CEN_N && lines < 40; i++) {
        if (!g_cen_fn[i]) continue;
        LONG c = g_cen_cnt[i], d = c - g_cen_last[i];
        if (d <= 0) continue;
        g_cen_last[i] = c; lines++;
        unsigned char by[16] = {0}; SIZE_T got = 0; void *r = g_cen_rdx[i];
        if ((ULONG_PTR)r > 0x10000) ReadProcessMemory(GetCurrentProcess(), r, by, 16, &got);
        float fl[4]; memcpy(fl, by, 16);
        L("[cen] %s #%d +%ld (total %ld) rdx=%p bytes=%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X floats=%.3f %.3f %.3f %.3f",
          g_cen_name[i / 64], i % 64, d, c, r, by[0],by[1],by[2],by[3],by[4],by[5],by[6],by[7],by[8],by[9],by[10],by[11],by[12],by[13],by[14],by[15],
          got ? fl[0] : 0.f, got ? fl[1] : 0.f, got ? fl[2] : 0.f, got ? fl[3] : 0.f);
    }
}


/* 0.1.4: dev.v0 slot 20 is called ~250/s with an object (vtable in GameInputRedist) in rdx:
 * almost certainly the raw output report. Dump it (log only) to find the DualSense bytes. */
static volatile LONG g_rawCalls, g_rawDumps; static DWORD g_rawLastTick; static DWORD g_rawLastHash;
static int user_ptr(ULONG_PTR q) { return q > 0x100000 && q < 0x7FF000000000ULL && !(q & 7); }
static void hexline(const char *tag, int off, const BYTE *b, int n)
{
    char t[3 * 32 + 1]; int k = 0;
    for (int i = 0; i < n; i++) k += sprintf(t + k, "%02X ", b[i]);
    L("[raw] %s +%03X: %s", tag, off, t);
}
static void raw_log(void *rep)
{
    LONG n = InterlockedIncrement(&g_rawCalls);
    if (!rep || g_rawDumps >= 120) return;
    BYTE ob[256]; SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), rep, ob, sizeof ob, &got) || got < 64) return;
    /* hash the object after its first 16 bytes plus whatever its pointers point at */
    BYTE pb[8][96]; int pn = 0; ULONG_PTR pv[8]; int poff[8];
    for (int o = 8; o < (int)got && pn < 8; o += 8) {
        ULONG_PTR q = *(ULONG_PTR *)(ob + o); SIZE_T g2 = 0;
        if (!user_ptr(q)) continue;
        if (ReadProcessMemory(GetCurrentProcess(), (void *)q, pb[pn], 96, &g2) && g2 == 96) { pv[pn] = q; poff[pn] = o; pn++; }
    }
    DWORD h = 2166136261u;
    for (int i = 16; i < (int)got; i++) h = (h * 16777619u) ^ ob[i];
    for (int j = 0; j < pn; j++) for (int i = 0; i < 96; i++) h = (h * 16777619u) ^ pb[j][i];
    DWORD now = GetTickCount();
    if (h == g_rawLastHash || (g_rawDumps > 0 && now - g_rawLastTick < 150)) return;
    g_rawLastHash = h; g_rawLastTick = now;
    LONG d = InterlockedIncrement(&g_rawDumps);
    if (d == 1) {
        void **vt = *(void ***)rep; char t[512]; int k = 0;
        for (int i = 0; i < 16; i++) k += sprintf(t + k, "%d:%p ", i, vt[i]);
        L("[raw] report vtable %p: %s", vt, t);
    }
    L("[raw] dump %ld at call %ld, object %p, %d pointers followed", d, n, rep, pn);
    for (int o = 0; o < 128; o += 32) hexline("obj", o, ob + o, 32);
    for (int j = 0; j < pn; j++) {
        char tag[32]; sprintf(tag, "obj+%02X->", poff[j]);
        for (int o = 0; o < 96; o += 32) hexline(tag, o, pb[j] + o, 32);
    }
}

/* Wwise: AkUniqueID of a name is FNV-1 32 of the lowercase name, so string posts map to the same ids. */
#define AKH 4096
static volatile LONG g_akId[AKH], g_akCnt[AKH], g_akWin[AKH], g_akUnique, g_akTotal, g_postLogs;
static char *volatile g_akName[AKH];
static volatile LONG g_akCine[AKH];
static DWORD ak_fnv(const void *s, int wide)
{
    DWORD h = 2166136261u;
    for (int i = 0; i < 256; i++) {
        unsigned c = wide ? ((const wchar_t *)s)[i] : ((const unsigned char *)s)[i];
        if (!c) break;
        if (c >= 'A' && c <= 'Z') c += 32;
        h = (h * 16777619u) ^ (c & 0xFF);
    }
    return h;
}
void ak_log(int k, void *a, void *b)
{
    if (k == 15) { raw_log(b); return; }
    (void)b;
    DWORD id; char nm[128] = "";
    if (g_ak_kind[k] == 0) id = (DWORD)(ULONG_PTR)a;
    else {
        if (!a) return;
        id = ak_fnv(a, g_ak_kind[k] == 2);
        if (g_ak_kind[k] == 1) { for (int i = 0; i < 127 && ((char *)a)[i]; i++) nm[i] = ((char *)a)[i]; }
        else { for (int i = 0; i < 127 && ((wchar_t *)a)[i]; i++) nm[i] = (char)((wchar_t *)a)[i]; }
    }
    if (!id) return;
    InterlockedIncrement(&g_akTotal);
    if (g_evN || g_lpN) ev_post(id);
    for (int j = 0, h = id & (AKH - 1); j < AKH; j++, h = (h + 1) & (AKH - 1)) {
        LONG cur = g_akId[h];
        if (cur == (LONG)id) {
            LONG c = InterlockedIncrement(&g_akCnt[h]); InterlockedIncrement(&g_akWin[h]);
            if (g_akCine[h] && C.cineMute) g_cineUntil = GetTickCount64() + (ULONGLONG)(C.cineHold * 1000);
            if (C.logPosts && c <= 20 && InterlockedIncrement(&g_postLogs) <= 6000) L("[post] %u%s%s", id, g_akName[h] ? " " : "", g_akName[h] ? g_akName[h] : "");
            return;
        }
        if (cur == 0 && InterlockedCompareExchange(&g_akId[h], (LONG)id, 0) == 0) {
            InterlockedIncrement(&g_akCnt[h]); InterlockedIncrement(&g_akWin[h]);
            LONG u = InterlockedIncrement(&g_akUnique);
            L("[ak] new event %u%s%s (unique #%ld)", id, nm[0] ? " name=" : "", nm, u);
            return;
        }
    }
}

static void ak_hook(void)
{
    BYTE *m = (BYTE *)GetModuleHandleW(NULL);
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(m + ((IMAGE_DOS_HEADER *)m)->e_lfanew);
    IMAGE_DATA_DIRECTORY dd = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!dd.VirtualAddress) { L("[ak] exe has no export table"); return; }
    IMAGE_EXPORT_DIRECTORY *ed = (IMAGE_EXPORT_DIRECTORY *)(m + dd.VirtualAddress);
    DWORD *names = (DWORD *)(m + ed->AddressOfNames); WORD *ords = (WORD *)(m + ed->AddressOfNameOrdinals);
    DWORD *fns = (DWORD *)(m + ed->AddressOfFunctions);
    int seen = 0;
    for (DWORD i = 0; i < ed->NumberOfNames && g_akN < 15; i++) {
        const char *n = (const char *)(m + names[i]);
        if (!strstr(n, "SoundEngine@AK@@")) continue;
        if (strncmp(n, "?PostEvent@", 11) && strncmp(n, "?ExecuteActionOnEvent@", 22)) continue;
        seen++;
        const char *p = strstr(n, "@@YA"); if (!p) continue;
        p += 4;
        if (*p == 'W') { p = strstr(p, "@@"); if (!p) continue; p += 2; } /* enum return, AKRESULT */
        else if (*p == 'I' || *p == 'J' || *p == 'K') p++;                /* AkPlayingID */
        else continue;
        int kind = (*p == 'I' || *p == 'K') ? 0 : !strncmp(p, "PEBD", 4) ? 1 : !strncmp(p, "PEB_W", 5) ? 2 : -1;
        void *f = m + fns[ords[i]];
        L("[ak] export %s kind=%s @%p", n, kind == 0 ? "id" : kind == 1 ? "char" : kind == 2 ? "wchar" : "skip", f);
        if (kind < 0) continue;
        int k = g_akN;
        g_ak_kind[k] = kind;
        MH_STATUS st = MH_CreateHook(f, ak_stub_table[k], &g_ak_orig[k]);
        if (st == MH_OK) st = MH_EnableHook(f);
        L("[ak] hook #%d: %s", k, MH_StatusToString(st));
        if (st == MH_OK) g_akN++;
    }
    L("[ak] %d PostEvent/ExecuteActionOnEvent exports seen, %d hooked", seen, g_akN);
}

/* ---- name scan: Wwise ids are FNV-1 32 of the lowercase event name. UE keeps asset
 * names (FName pool) in memory, so hash every identifier-like string and match. ---- */
#define CATN (1 << 16)
static DWORD g_catId[CATN]; static char *g_catName[CATN]; static int g_catN;
static int ak_find(DWORD id)
{
    for (int j = 0, h = id & (AKH - 1); j < AKH; j++, h = (h + 1) & (AKH - 1)) {
        if ((DWORD)g_akId[h] == id) return h;
        if (!g_akId[h]) return -1;
    }
    return -1;
}
static int cat_prefix(const char *s, int n)
{
    static const char *px[] = { "play_", "stop_", "ui_", "sfx_", "mx_", "vo_", "amb_", "wpn_", "fol_" };
    for (int i = 0; i < (int)(sizeof px / sizeof px[0]); i++) {
        int k = (int)strlen(px[i]); if (n <= k) continue;
        int ok = 1; for (int j = 0; j < k && ok; j++) { char c = s[j]; if (c >= 'A' && c <= 'Z') c += 32; if (c != px[i][j]) ok = 0; }
        if (ok) return 1;
    }
    return 0;
}
static void name_hit(const char *s, int n)
{
    DWORD h = 2166136261u;
    for (int i = 0; i < n; i++) { unsigned c = (unsigned char)s[i]; if (c >= 'A' && c <= 'Z') c += 32; h = (h * 16777619u) ^ c; }
    int seen = ak_find(h);
    if (seen >= 0 && !g_akName[seen]) {
        char *d = malloc(n + 1); memcpy(d, s, n); d[n] = 0; g_akName[seen] = d;
        if ((n > 5 && !_stricmp(d + n - 5, "_CINE")) || strstr(d, "CineDX")) g_akCine[seen] = 1;
        L("[name] %u = %s (posted %ld times so far)", h, d, g_akCnt[seen]);
    }
    if (seen < 0 && !cat_prefix(s, n)) return;
    for (int j = 0, k = h & (CATN - 1); j < CATN; j++, k = (k + 1) & (CATN - 1)) {
        if (g_catId[k] == h) return;
        if (!g_catId[k]) {
            if (g_catN >= CATN * 3 / 4) return;
            char *d = malloc(n + 1); memcpy(d, s, n); d[n] = 0;
            g_catName[k] = d; g_catId[k] = h; g_catN++; return;
        }
    }
}
static void name_scan(int pass)
{
    DWORD t0 = GetTickCount(); SIZE_T scanned = 0; int before = g_catN;
    static BYTE buf[(1 << 20) + 128];
    MEMORY_BASIC_INFORMATION mi;
    for (BYTE *a = (BYTE *)0x10000; a < (BYTE *)0x7FFFFFFF0000ULL; a = (BYTE *)mi.BaseAddress + mi.RegionSize) {
        if (!VirtualQuery(a, &mi, sizeof mi)) break;
        DWORD pr = mi.Protect;
        if (mi.State != MEM_COMMIT || (pr & (PAGE_GUARD | PAGE_NOACCESS | PAGE_NOCACHE | PAGE_WRITECOMBINE))) continue;
        if (!(pr & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))) continue;
        if (mi.Type == MEM_MAPPED || mi.RegionSize > ((SIZE_T)1 << 30)) continue;
        for (SIZE_T off = 0; off < mi.RegionSize; off += (1 << 20)) {
            SIZE_T want = mi.RegionSize - off; if (want > (1 << 20) + 128) want = (1 << 20) + 128;
            if (off + want > mi.RegionSize) want = mi.RegionSize - off;
            SIZE_T got = 0;
            if (!ReadProcessMemory(GetCurrentProcess(), (BYTE *)mi.BaseAddress + off, buf, want, &got) || !got) continue;
            scanned += got;
            int run = 0;
            SIZE_T lim = got > (1 << 20) ? (1 << 20) : got;   /* start runs only inside the chunk, finish them in the overlap */
            for (SIZE_T i = 0; i < got; i++) {
                BYTE c = buf[i];
                int idc = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
                if (idc) { if (run || i < lim) run++; if (run > 100) run = 101; continue; }
                if (run >= 5 && run <= 100) name_hit((const char *)buf + i - run, run);
                run = 0;
                if (i >= lim) break;
            }
        }
        Sleep(0);
    }
    int named = 0; for (int h = 0; h < AKH; h++) if (g_akId[h] && g_akName[h]) named++;
    L("[name] scan %d: %.0f MB in %lu ms, %d of %ld posted ids named, catalog %d (+%d)", pass, scanned / 1048576.0, GetTickCount() - t0, named, g_akUnique, g_catN, g_catN - before);
    wchar_t p[MAX_PATH]; wcscpy(p, g_dir); wcscat(p, L"GearsHaptics_catalog.txt");
    FILE *f = _wfopen(p, L"w");
    if (!f) return;
    fprintf(f, "# Gears Haptics %s event catalog, scan %d. id, times posted this session, name\n", LANTERN_VERSION, pass);
    for (int k = 0; k < CATN; k++) if (g_catId[k]) { int s2 = ak_find(g_catId[k]); fprintf(f, "%u\t%ld\t%s\n", g_catId[k], s2 >= 0 ? g_akCnt[s2] : 0L, g_catName[k]); }
    for (int h = 0; h < AKH; h++) if (g_akId[h] && !g_akName[h]) fprintf(f, "%u\t%ld\t?\n", (DWORD)g_akId[h], g_akCnt[h]);
    fclose(f);
}
static DWORD WINAPI scan_thread(void *arg)
{
    (void)arg;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
    static const DWORD at[3] = { 60000, 180000, 360000 };
    DWORD last = 0;
    for (int i = 0; i < 3; i++) { Sleep(at[i] - last); last = at[i]; name_scan(i + 1); }
    return 0;
}

static void ak_report(void)
{
    if (!g_akN) return;
    char buf[1024]; int len = 0, shown = 0;
    for (int pass = 0; pass < 12; pass++) {
        int best = -1; LONG bw = 0;
        for (int h = 0; h < AKH; h++) if (g_akId[h] && g_akWin[h] > bw) { bw = g_akWin[h]; best = h; }
        if (best < 0) break;
        len += snprintf(buf + len, sizeof buf - len, " %u x%ld", (DWORD)g_akId[best], bw);
        g_akWin[best] = -g_akWin[best] - 1; shown++; /* hide from next pass */
    }
    for (int h = 0; h < AKH; h++) { if (g_akWin[h] < 0) g_akWin[h] = 0; else if (g_akWin[h] > 0) g_akWin[h] = 0; }
    if (shown) L("[ak] 2s top:%s  (unique %ld, total %ld)", buf, g_akUnique, g_akTotal);
}

static HMODULE (WINAPI *oLoadLibraryExW)(LPCWSTR, HANDLE, DWORD);
static HMODULE WINAPI hLoadLibraryExW(LPCWSTR n, HANDLE f, DWORD fl)
{
    HMODULE m = oLoadLibraryExW(n, f, fl);
    if (m && n && !t_inHook && !(fl & 0x62) /* skip datafile / image-resource mappings */) {
        const wchar_t *b = wcsrchr(n, L'\\'); b = b ? b + 1 : n;
        if (!_wcsnicmp(b, L"GameInput", 9)) { t_inHook++; gi_hook_module(m, b); t_inHook--; }
    }
    return m;
}

static DWORD WINAPI gi_thread(void *arg)
{
    (void)arg;
    for (int t = 0;; t++) {
        HMODULE m = GetModuleHandleW(L"GameInputRedist.dll");
        if (!m) m = GetModuleHandleW(L"GameInput.dll");
        if (m) gi_hook_module(m, L"(found loaded)");
        xi_scan();
        if (t == 15 && C.hookWwise) { ak_hook(); if (g_akN && C.nameScan) CloseHandle(CreateThread(NULL, 0, scan_thread, NULL, 0, NULL)); }
        if (C.census && (pGICreate || pGIInit)) {
            int pending = 0;
            for (int v = 0; v < 5; v++) if (g_giHooked[v] == 0) pending = 1;
            if (pending) gi_probe();
        }
        Sleep(t < 60 ? 1000 : 3000);
    }
}

/* ---------------- 0.2.4: which controller the game uses ----------------
 * The game's HID path is stored here (cheap, inside the CreateFile hook). The watchdog
 * thread turns it into a container id later, so the haptics stream can pick the audio
 * endpoint of that same controller instead of the first "Wireless Controller" in the list. */
static wchar_t g_dsPath[512];
static volatile LONG g_dsPathSeq;
static SRWLOCK g_dsPathLock = SRWLOCK_INIT;
static GUID g_dsContainer; static volatile LONG g_dsContainerOk;
static void ds_path_seen(const wchar_t *p)
{
    AcquireSRWLockExclusive(&g_dsPathLock);
    if (_wcsicmp(g_dsPath, p)) { wcsncpy(g_dsPath, p, 511); g_dsPath[511] = 0; InterlockedIncrement(&g_dsPathSeq); }
    ReleaseSRWLockExclusive(&g_dsPathLock);
}
static int hid_container(const wchar_t *path, GUID *out)
{
    wchar_t inst[512]; ULONG sz = sizeof inst; DEVPROPTYPE t; DEVINST di;
    if (CM_Get_Device_Interface_PropertyW(path, &DEVPKEY_Device_InstanceId, &t, (PBYTE)inst, &sz, 0) != CR_SUCCESS) return 0;
    if (CM_Locate_DevNodeW(&di, inst, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) return 0;
    sz = sizeof *out;
    if (CM_Get_DevNode_PropertyW(di, &DEVPKEY_Device_ContainerId, &t, (PBYTE)out, &sz, 0) != CR_SUCCESS) return 0;
    return 1;
}

/* ---------------- hooks ---------------- */
static HANDLE (WINAPI *oCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
static HANDLE (WINAPI *oCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
static BOOL (WINAPI *oWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
static BOOL (WINAPI *oDeviceIoControl)(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
static BOOL (WINAPI *oCloseHandle)(HANDLE);

/* forget closed handles so a reused handle value is never treated as a DualSense */
static BOOL WINAPI hCloseHandle(HANDLE h)
{
    AcquireSRWLockShared(&g_hLock);
    int hit = -1;
    for (int i = 0; i < MAXH; i++) if (g_h[i].h == h) { hit = i; break; }
    ReleaseSRWLockShared(&g_hLock);
    if (hit >= 0) {
        AcquireSRWLockExclusive(&g_hLock);
        if (g_h[hit].h == h) { if (g_h[hit].ds) L("[hid] DualSense handle %p closed", h); g_h[hit].h = NULL; g_h[hit].ds = 0; }
        ReleaseSRWLockExclusive(&g_hLock);
    }
    return oCloseHandle(h);
}

static HANDLE WINAPI hCreateFileW(LPCWSTR p, DWORD a, DWORD s, LPSECURITY_ATTRIBUTES sa, DWORD d, DWORD f, HANDLE t)
{
    HANDLE h = oCreateFileW(p, a, s, sa, d, f, t);
    if (h != INVALID_HANDLE_VALUE && !t_inHook && p && p[0] == L'\\' && p[1] == L'\\') {
        if (is_ds_path_w(p)) { remember(h, 1); ds_path_seen(p); L("[hid] game opened DualSense: %ls (access 0x%lX, flags 0x%lX) -> %p", p, a, f, h); }
    }
    return h;
}

static HANDLE WINAPI hCreateFileA(LPCSTR p, DWORD a, DWORD s, LPSECURITY_ATTRIBUTES sa, DWORD d, DWORD f, HANDLE t)
{
    HANDLE h = oCreateFileA(p, a, s, sa, d, f, t);
    if (h != INVALID_HANDLE_VALUE && !t_inHook && p && p[0] == '\\' && p[1] == '\\') {
        wchar_t w[512]; MultiByteToWideChar(CP_ACP, 0, p, -1, w, 512);
        if (is_ds_path_w(w)) { remember(h, 1); ds_path_seen(w); L("[hid] game opened DualSense (A): %s -> %p", p, h); }
    }
    return h;
}

static BOOL WINAPI hWriteFile(HANDLE h, LPCVOID buf, DWORD n, LPDWORD wr, LPOVERLAPPED ov)
{
    if (!t_inHook && buf && n >= 48 && n <= 96) {
        BYTE first = ((const BYTE *)buf)[0];
        if ((first == 0x02 || first == 0x31) && handle_is_ds(h)) {
            InterlockedIncrement(&s_writes);
            process_report((BYTE *)buf, n, "[hid] WriteFile");
        }
    }
    return oWriteFile(h, buf, n, wr, ov);
}

#define IOCTL_HID_SET_OUTPUT_REPORT_ 0x000B0195
static BOOL WINAPI hDeviceIoControl(HANDLE h, DWORD code, LPVOID in, DWORD inN, LPVOID out, DWORD outN, LPDWORD ret, LPOVERLAPPED ov)
{
    if (!t_inHook && code == IOCTL_HID_SET_OUTPUT_REPORT_ && in && inN >= 48 && handle_is_ds(h)) {
        InterlockedIncrement(&s_ioctl);
        process_report((BYTE *)in, inN, "[hid] SetOutputReport");
    }
    return oDeviceIoControl(h, code, in, inN, out, outN, ret, ov);
}

static int hook(HMODULE m, const char *name, void *det, void **orig)
{
    void *target = m ? (void *)GetProcAddress(m, name) : NULL;
    if (!target) { L("hook %s: not found", name); return 0; }
    MH_STATUS s = MH_CreateHook(target, det, orig);
    if (s == MH_OK) s = MH_EnableHook(target);
    L("hook %s @%p: %s", name, target, MH_StatusToString(s));
    return s == MH_OK;
}

static void install_hooks(void)
{
    HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
    HMODULE hid = LoadLibraryW(L"hid.dll");
    if (hid) pHidD_GetAttributes = (void *)GetProcAddress(hid, "HidD_GetAttributes");
    MH_STATUS s = MH_Initialize();
    L("MinHook init: %s", MH_StatusToString(s));
    if (s != MH_OK) return;
    hook(kb, "CreateFileW", hCreateFileW, (void **)&oCreateFileW);
    hook(kb, "CreateFileA", hCreateFileA, (void **)&oCreateFileA);
    hook(kb, "WriteFile", hWriteFile, (void **)&oWriteFile);
    hook(kb, "DeviceIoControl", hDeviceIoControl, (void **)&oDeviceIoControl);
    hook(kb, "CloseHandle", hCloseHandle, (void **)&oCloseHandle);
    hook(kb, "LoadLibraryExW", hLoadLibraryExW, (void **)&oLoadLibraryExW);
}


/* ---------------- game audio stream tap (diagnostic, log only) ----------------
 * Hypothesis: the game makes its DualSense rumble as audio on the controller's
 * own endpoint (channels 3/4), not in HID reports. Watch only streams opened on
 * the controller endpoint (or on an unknown device with 4+ channels) and log
 * per channel peaks. Nothing here touches the coils or the game's audio. */
#define MAXS 32
typedef struct {
    void *client, *render; int ds, known, ch, bits, isFloat; DWORD mask; BYTE *buf;
    volatile LONG nbuf, nsilent; float peak[8]; wchar_t dev[96];
} Stream;
static Stream g_s[MAXS];
static SRWLOCK g_sLock = SRWLOCK_INIT;
static void *g_ourClient, *g_ourRender;

static Stream *s_find(void *client, void *render, int create)
{
    Stream *r = NULL;
    AcquireSRWLockExclusive(&g_sLock);
    for (int i = 0; i < MAXS && !r; i++)
        if ((client && g_s[i].client == client) || (render && g_s[i].render == render)) r = &g_s[i];
    if (!r && create) for (int i = 0; i < MAXS && !r; i++) if (!g_s[i].client) { r = &g_s[i]; memset(r, 0, sizeof(*r)); r->client = client; }
    ReleaseSRWLockExclusive(&g_sLock);
    return r;
}

static HRESULT (STDMETHODCALLTYPE *oActivate)(IMMDevice *, REFIID, DWORD, PROPVARIANT *, void **);
static HRESULT (STDMETHODCALLTYPE *oInit)(IAudioClient *, AUDCLNT_SHAREMODE, DWORD, REFERENCE_TIME, REFERENCE_TIME, const WAVEFORMATEX *, LPCGUID);
static HRESULT (STDMETHODCALLTYPE *oGetService)(IAudioClient *, REFIID, void **);
static HRESULT (STDMETHODCALLTYPE *oGetBuffer)(IAudioRenderClient *, UINT32, BYTE **);
static HRESULT (STDMETHODCALLTYPE *oRelBuffer)(IAudioRenderClient *, UINT32, DWORD);

static void dev_name(IMMDevice *d, wchar_t *out, int n)
{
    out[0] = 0;
    IPropertyStore *ps = NULL;
    if (SUCCEEDED(d->lpVtbl->OpenPropertyStore(d, STGM_READ, &ps))) {
        PROPVARIANT v; PropVariantInit(&v);
        if (SUCCEEDED(ps->lpVtbl->GetValue(ps, &PKEY_Device_FriendlyName, &v)) && v.vt == VT_LPWSTR) { wcsncpy(out, v.pwszVal, n - 1); out[n - 1] = 0; }
        PropVariantClear(&v);
        ps->lpVtbl->Release(ps);
    }
}

static HRESULT STDMETHODCALLTYPE hActivate(IMMDevice *d, REFIID riid, DWORD ctx, PROPVARIANT *pa, void **out)
{
    HRESULT hr = oActivate(d, riid, ctx, pa, out);
    if (SUCCEEDED(hr) && out && *out && !t_inHook) {
        wchar_t nm[96]; t_inHook++; dev_name(d, nm, 96); t_inHook--;
        int isClient = IsEqualIID(riid, &IID_IAudioClient);
#ifdef __IAudioClient2_INTERFACE_DEFINED__
        isClient |= IsEqualIID(riid, &IID_IAudioClient2);
#endif
#ifdef __IAudioClient3_INTERFACE_DEFINED__
        isClient |= IsEqualIID(riid, &IID_IAudioClient3);
#endif
        L("[tap] game Activate on \"%ls\" -> %p%s", nm, *out, isClient ? " (audio client)" : "");
        if (isClient) {
            Stream *st = s_find(*out, NULL, 1);
            if (st) { st->known = 1; st->ds = wcsstr(nm, L"Wireless Controller") != NULL; wcsncpy(st->dev, nm, 95); }
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE hInit(IAudioClient *c, AUDCLNT_SHAREMODE sm, DWORD fl, REFERENCE_TIME dur, REFERENCE_TIME per, const WAVEFORMATEX *wf, LPCGUID sess)
{
    HRESULT hr = oInit(c, sm, fl, dur, per, wf, sess);
    if ((void *)c != g_ourClient && wf) {
        Stream *st = s_find(c, NULL, 1);
        if (st) {
            st->ch = wf->nChannels; st->bits = wf->wBitsPerSample; st->isFloat = wf->wFormatTag == WAVE_FORMAT_IEEE_FLOAT; st->mask = 0;
            if (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
                const WAVEFORMATEXTENSIBLE *x = (const WAVEFORMATEXTENSIBLE *)wf;
                st->mask = x->dwChannelMask; st->isFloat = IsEqualGUID(&x->SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
            }
            L("[tap] Initialize client %p dev=\"%ls\" %s flags=0x%lX %lu Hz %d ch %d bit %s mask 0x%lX -> 0x%08lX",
              c, st->known ? st->dev : L"(unknown, not via IMMDevice)", sm == AUDCLNT_SHAREMODE_EXCLUSIVE ? "EXCLUSIVE" : "shared", fl,
              wf->nSamplesPerSec, wf->nChannels, wf->wBitsPerSample, st->isFloat ? "float" : "pcm", st->mask, hr);
        }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE hGetService(IAudioClient *c, REFIID riid, void **out)
{
    HRESULT hr = oGetService(c, riid, out);
    if (SUCCEEDED(hr) && out && *out && (void *)c != g_ourClient && IsEqualIID(riid, &IID_IAudioRenderClient)) {
        Stream *st = s_find(c, NULL, 0);
        if (st) { st->render = *out; L("[tap] render client %p for client %p (%ls)", *out, c, st->known ? st->dev : L"unknown dev"); }
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE hGetBuffer(IAudioRenderClient *r, UINT32 n, BYTE **pp)
{
    HRESULT hr = oGetBuffer(r, n, pp);
    if (SUCCEEDED(hr) && (void *)r != g_ourRender && pp) {
        Stream *st = s_find(NULL, r, 0);
        if (st) st->buf = *pp;
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE hRelBuffer(IAudioRenderClient *r, UINT32 n, DWORD fl)
{
    if ((void *)r != g_ourRender) {
        Stream *st = s_find(NULL, r, 0);
        /* only the controller endpoint, or an unknown device with 4+ channels */
        if (st && st->buf && n && (st->ds || (!st->known && st->ch >= 4))) {
            InterlockedIncrement(&st->nbuf);
            if (fl & AUDCLNT_BUFFERFLAGS_SILENT) InterlockedIncrement(&st->nsilent);
            else {
                int ch = st->ch > 8 ? 8 : st->ch;
                for (UINT32 i = 0; i < n; i++) for (int c = 0; c < ch; c++) {
                    float v = 0;
                    if (st->isFloat && st->bits == 32) v = ((float *)st->buf)[(size_t)i * st->ch + c];
                    else if (st->bits == 16) v = ((short *)st->buf)[(size_t)i * st->ch + c] / 32768.f;
                    else if (st->bits == 32) v = ((int *)st->buf)[(size_t)i * st->ch + c] / 2147483648.f;
                    v = fabsf(v); if (v > st->peak[c]) st->peak[c] = v;
                }
            }
        }
    }
    return oRelBuffer(r, n, fl);
}

static void tap_stats(void)
{
    for (int i = 0; i < MAXS; i++) {
        Stream *st = &g_s[i];
        if (!st->client || !st->nbuf) continue;
        char pk[128]; int k = 0;
        for (int c = 0; c < st->ch && c < 8; c++) { k += sprintf(pk + k, "ch%d=%.3f ", c + 1, st->peak[c]); st->peak[c] = 0; }
        L("[tap] stream %p (%ls) buffers=%ld silent=%ld peaks: %s", st->client, st->known ? st->dev : L"unknown dev", st->nbuf, st->nsilent, pk);
        st->nbuf = 0; st->nsilent = 0;
    }
}

static void install_audio_hooks(IMMDeviceEnumerator *en)
{
    IMMDevice *d = NULL; IAudioClient *c = NULL; IAudioRenderClient *r = NULL; WAVEFORMATEX *wf = NULL;
    if (FAILED(en->lpVtbl->GetDefaultAudioEndpoint(en, eRender, eConsole, &d))) { L("[tap] no default endpoint, tap off"); return; }
    void *fAct = d->lpVtbl->Activate;
    if (SUCCEEDED(d->lpVtbl->Activate(d, &IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&c)) && SUCCEEDED(c->lpVtbl->GetMixFormat(c, &wf)) &&
        SUCCEEDED(c->lpVtbl->Initialize(c, AUDCLNT_SHAREMODE_SHARED, 0, 300000, 0, wf, NULL)) &&
        SUCCEEDED(c->lpVtbl->GetService(c, &IID_IAudioRenderClient, (void **)&r))) {
        int ok = 1;
        ok &= MH_CreateHook(fAct, hActivate, (void **)&oActivate) == MH_OK;
        ok &= MH_CreateHook(c->lpVtbl->Initialize, hInit, (void **)&oInit) == MH_OK;
        ok &= MH_CreateHook(c->lpVtbl->GetService, hGetService, (void **)&oGetService) == MH_OK;
        ok &= MH_CreateHook(r->lpVtbl->GetBuffer, hGetBuffer, (void **)&oGetBuffer) == MH_OK;
        ok &= MH_CreateHook(r->lpVtbl->ReleaseBuffer, hRelBuffer, (void **)&oRelBuffer) == MH_OK;
        MH_STATUS e = MH_EnableHook(MH_ALL_HOOKS);
        L("[tap] audio hooks: create %s, enable %s", ok ? "ok" : "FAILED", MH_StatusToString(e));
    } else L("[tap] could not build a probe client, tap off");
    if (r) r->lpVtbl->Release(r);
    if (wf) CoTaskMemFree(wf);
    if (c) c->lpVtbl->Release(c);
    d->lpVtbl->Release(d);
}

/* ---------------- haptic synth (WASAPI) ---------------- */
#define SPK_BL 0x10
#define SPK_BR 0x20
#define SPK_SL 0x200
#define SPK_SR 0x400

static int popcount(DWORD v) { int c = 0; while (v) { c += v & 1; v >>= 1; } return c; }
static int chan_index(DWORD mask, DWORD bit, DWORD alt, int fb)
{
    DWORD b = (mask & bit) ? bit : (mask & alt) ? alt : 0;
    if (!b) return fb;
    return popcount(mask & (b - 1));
}

static const PROPERTYKEY PK_ContainerId = { {0x8c7ed206,0x3f8a,0x4827,{0xb3,0xab,0xae,0x9e,0x1f,0xae,0xfc,0x6c}}, 2 };

/* 0.2.4: pick the controller endpoint. With more than one "Wireless Controller" (a second pad,
 * DSX or DSZ, a replugged pad), prefer the one in the same container as the HID device the
 * game opened. Falls back to the first one. idOut gets the endpoint id (CoTaskMemFree it). */
static IMMDevice *find_ds_endpoint(IMMDeviceEnumerator *en, wchar_t *nameOut, wchar_t **idOut, int quiet)
{
    IMMDeviceCollection *col = NULL; IMMDevice *first = NULL, *match = NULL; int count = 0;
    wchar_t firstName[256] = L"", matchName[256] = L"";
    if (idOut) *idOut = NULL;
    if (FAILED(IMMDeviceEnumerator_EnumAudioEndpoints(en, eRender, DEVICE_STATE_ACTIVE, &col))) return NULL;
    UINT n = 0; IMMDeviceCollection_GetCount(col, &n);
    int haveC = g_dsContainerOk; GUID want = g_dsContainer;
    for (UINT i = 0; i < n; i++) {
        IMMDevice *d = NULL; if (FAILED(IMMDeviceCollection_Item(col, i, &d))) continue;
        IPropertyStore *ps = NULL; int keep = 0;
        if (SUCCEEDED(IMMDevice_OpenPropertyStore(d, STGM_READ, &ps))) {
            PROPVARIANT v; PropVariantInit(&v);
            if (SUCCEEDED(IPropertyStore_GetValue(ps, &PKEY_Device_FriendlyName, &v)) && v.vt == VT_LPWSTR && wcsstr(v.pwszVal, L"Wireless Controller")) {
                count++;
                if (!first) { first = d; keep = 1; wcsncpy(firstName, v.pwszVal, 255); }
                if (haveC && !match) {
                    PROPVARIANT c; PropVariantInit(&c);
                    if (SUCCEEDED(IPropertyStore_GetValue(ps, &PK_ContainerId, &c)) && c.vt == VT_CLSID && c.puuid && IsEqualGUID(c.puuid, &want)) {
                        match = d; keep = 1; wcsncpy(matchName, v.pwszVal, 255);
                    }
                    PropVariantClear(&c);
                }
            }
            PropVariantClear(&v);
            IPropertyStore_Release(ps);
        }
        if (!keep) IMMDevice_Release(d);
    }
    IMMDeviceCollection_Release(col);
    IMMDevice *found = match ? match : first;
    if (match && first && first != match) IMMDevice_Release(first);
    if (found) {
        wcsncpy(nameOut, match ? matchName : firstName, 255); nameOut[255] = 0;
        if (idOut) IMMDevice_GetId(found, idOut);
        if (!quiet && count > 1) L("[audio] %d controller audio endpoints found, using %s", count, match ? "the one that matches the game's controller" : "the first one (game's controller not known yet)");
    }
    return found;
}

/* stream health, shared with the watchdog */
static volatile LONG g_rebuild;
static wchar_t g_curEp[512];
static SRWLOCK g_curEpLock = SRWLOCK_INIT;

/* our own audio session: Windows remembers per app volume and mute, and with a NULL session
 * the haptics shared the game's own session on the controller, so a muted or lowered game
 * session (kept by Windows across launches) silenced the haptics too. */
static const GUID GH_SESSION = {0x6a1c3b52,0x9d4e,0x4f17,{0x8a,0x2b,0x5e,0x91,0x0c,0x7d,0x33,0xe4}};

static void endpoint_volume_check(IMMDevice *dev)
{
    IAudioEndpointVolume *ev = NULL;
    t_inHook++;
    HRESULT hr = IMMDevice_Activate(dev, &IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void **)&ev);
    t_inHook--;
    if (FAILED(hr) || !ev) return;
    BOOL mute = FALSE; float lvl = 1.f;
    IAudioEndpointVolume_GetMute(ev, &mute);
    IAudioEndpointVolume_GetMasterVolumeLevelScalar(ev, &lvl);
    L("[audio] controller endpoint volume %.0f%%%s", lvl * 100, mute ? ", MUTED" : "");
    if (mute || lvl < 0.05f) L("[audio] the Wireless Controller device is muted or at 0 in Windows sound settings, haptics will be silent. Turn it back up.");
    IAudioEndpointVolume_Release(ev);
}

typedef struct { double ph; float amp, decay, hz; int left, side; } Voice;
typedef struct { double phL, phH, phT; float low, high, tr, lastSum; int chime; LONG evR; Voice v[8];
    ULONGLONG lpNext[MAXLOOP], lpDub[MAXLOOP], lpQuiet[MAXLOOP]; int lpAlt; } Synth;

static void voice_start(Synth *S, int preset, float amp, int side, int sr)
{
    int slot = 0;
    for (int k = 1; k < 8; k++) if (S->v[k].left < S->v[slot].left) slot = k;
    const Preset *pr = &g_presets[preset];
    Voice *vo = &S->v[slot];
    vo->ph = 0; vo->hz = pr->hz; vo->amp = amp;
    vo->decay = expf(-1.f / (sr * pr->decayMs / 1000.f)); vo->left = (int)(sr * pr->lenMs / 1000.f); vo->side = side;
    if (vo->side == 3) { S->lpAlt ^= 1; vo->side = S->lpAlt ? 1 : 2; }
}

static void loops_run(Synth *S, int sr)
{
    if (!g_lpN) return;
    ULONGLONG now = GetTickCount64();
    float rum = g_low + g_high;
    for (int g = 0; g < g_lpGroups; g++) {
        LONG on = g_lpOn[g];
        if (!on) { S->lpNext[g] = 0; continue; }
        LoopDef *d = &g_lp[on - 1];
        ULONGLONG t0 = g_lpT0[g];
        if (now - t0 > (ULONGLONG)(d->maxSec * 1000)) { InterlockedCompareExchange(&g_lpOn[g], 0, on); continue; }
        if (C.cineMute && now < g_cineUntil && d->cat != CAT_MENUS) { InterlockedCompareExchange(&g_lpOn[g], 0, on); continue; }
        if (d->untilRumble && now - t0 > 150) {
            if (rum < 0.02f) { if (!S->lpQuiet[g]) S->lpQuiet[g] = now; else if (now - S->lpQuiet[g] > 200) { InterlockedCompareExchange(&g_lpOn[g], 0, on); S->lpQuiet[g] = 0; continue; } }
            else S->lpQuiet[g] = 0;
        }
        if (!S->lpNext[g] || S->lpNext[g] < t0) S->lpNext[g] = t0;
        if (now >= S->lpNext[g]) {
            voice_start(S, d->preset, d->gain * C.evGain, d->side, sr);
            if (d->dubMs > 0) S->lpDub[g] = now + (ULONGLONG)d->dubMs;
            S->lpNext[g] = now + (ULONGLONG)d->periodMs;
        }
        if (S->lpDub[g] && now >= S->lpDub[g]) { voice_start(S, d->preset, d->gain * C.evGain * 0.6f, d->side, sr); S->lpDub[g] = 0; }
    }
}

static void synth_fill(Synth *S, BYTE *out, UINT frames, int ch, int isFloat, int bits, int iL, int iR, int sr)
{
    float str = C.enabled ? C.strength : 0.f;
    float tl = g_low, th = g_high;
    if (tl > 0) tl = powf(tl, C.curve);
    if (th > 0) th = powf(th, C.curve);
    float sum = tl + th;
    /* start queued sound event voices */
    while (S->evR < g_evW) {
        LONG e = InterlockedExchange(&g_evQ[S->evR & 63], 0);
        if (!e) break;
        S->evR++;
        int ei = e - 1;
        voice_start(S, g_ev[ei].preset, g_ev[ei].gain * C.evGain, g_ev[ei].side, sr);
    }
    loops_run(S, sr);
    if (sum - S->lastSum > C.trThresh) { S->tr += (sum - S->lastSum) * C.trGain; if (S->tr > 1) S->tr = 1; }
    S->lastSum = sum;
    float atk = 1.f - expf(-1.f / (sr * 0.004f));
    float rel = 1.f - expf(-1.f / (sr * 0.045f));
    float trDecay = expf(-1.f / (sr * 0.025f));
    double dL = 2 * M_PI * C.lowHz / sr, dH = 2 * M_PI * C.highHz / sr, dT = 2 * M_PI * 140.0 / sr;
    for (UINT i = 0; i < frames; i++) {
        S->low += (tl - S->low) * (tl > S->low ? atk : rel);
        S->high += (th - S->high) * (th > S->high ? atk : rel);
        S->phL += dL; S->phH += dH; S->phT += dT;
        if (S->phL > 2 * M_PI) S->phL -= 2 * M_PI;
        if (S->phH > 2 * M_PI) S->phH -= 2 * M_PI;
        if (S->phT > 2 * M_PI) S->phT -= 2 * M_PI;
        float sL = (float)sin(S->phL) * S->low * C.lowGain;
        float sH = (float)sin(S->phH) * S->high * C.highGain;
        float sT = (float)sin(S->phT) * S->tr;
        S->tr *= trDecay;
        float eL = 0, eR = 0;
        for (int k = 0; k < 8; k++) {
            Voice *vo = &S->v[k];
            if (vo->left <= 0) continue;
            vo->left--;
            vo->ph += 2 * M_PI * vo->hz / sr;
            float x = (float)sin(vo->ph) * vo->amp;
            vo->amp *= vo->decay;
            if (vo->side != 2) eL += x;
            if (vo->side != 1) eR += x;
        }
        if (S->chime > 0) { sT += (float)sin(S->phH * 1.3) * 0.5f; S->chime--; }
        float left = tanhf((sL + sH * C.bleed + sT + eL) * str * 1.4f);
        float right = tanhf((sH + sL * C.bleed + sT + eR) * str * 1.4f);
        if (isFloat) {
            float *f = (float *)out + (size_t)i * ch;
            for (int c = 0; c < ch; c++) f[c] = 0.f;
            f[iL] = left; f[iR] = right;
        } else if (bits == 16) {
            short *f = (short *)out + (size_t)i * ch;
            for (int c = 0; c < ch; c++) f[c] = 0;
            f[iL] = (short)(left * 32767); f[iR] = (short)(right * 32767);
        } else if (bits == 32) {
            int *f = (int *)out + (size_t)i * ch;
            for (int c = 0; c < ch; c++) f[c] = 0;
            f[iL] = (int)(left * 2147483000.0); f[iR] = (int)(right * 2147483000.0);
        }
    }
}

static int run_stream(IMMDeviceEnumerator *en)
{
    wchar_t name[256] = L""; wchar_t *epId = NULL;
    IMMDevice *dev = find_ds_endpoint(en, name, &epId, 0);
    if (!dev) return 0;
    IAudioClient *ac = NULL; IAudioRenderClient *rc = NULL; ISimpleAudioVolume *sv = NULL; WAVEFORMATEX *mix = NULL; int ok = 0;
    InterlockedExchange(&g_rebuild, 0);
    AcquireSRWLockExclusive(&g_curEpLock);
    wcsncpy(g_curEp, epId ? epId : L"", 511); g_curEp[511] = 0;
    ReleaseSRWLockExclusive(&g_curEpLock);
    t_inHook++;
    HRESULT ahr = IMMDevice_Activate(dev, &IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&ac);
    t_inHook--;
    if (FAILED(ahr)) goto done;
    g_ourClient = ac;
    if (FAILED(IAudioClient_GetMixFormat(ac, &mix))) goto done;
    int ch = mix->nChannels, sr = mix->nSamplesPerSec, bits = mix->wBitsPerSample, isFloat = 0;
    DWORD mask = 0;
    if (mix->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        WAVEFORMATEXTENSIBLE *x = (WAVEFORMATEXTENSIBLE *)mix;
        mask = x->dwChannelMask;
        isFloat = IsEqualGUID(&x->SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
    } else if (mix->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) isFloat = 1;
    L("[audio] DualSense endpoint: %ls (%d Hz, %d ch, %d bit, %s, mask 0x%lX)", name, sr, ch, bits, isFloat ? "float" : "pcm", mask);
    if (ch < 4) { L("[audio] endpoint has %d channels, need 4 (is the controller on USB? Is the Wireless Controller device set to 4 channels in Windows sound settings?)", ch); goto done; }
    if (popcount(mask) != ch) mask = (ch == 4) ? (0x1 | 0x2 | SPK_BL | SPK_BR) : 0;
    int iL = chan_index(mask, SPK_BL, SPK_SL, 2), iR = chan_index(mask, SPK_BR, SPK_SR, 3);
    if (iL >= ch || iR >= ch) { iL = 2; iR = 3; }
    L("[audio] haptic channels: left actuator ch%d, right actuator ch%d", iL + 1, iR + 1);
    endpoint_volume_check(dev);

    REFERENCE_TIME dur = 300000; /* 30 ms */
    HRESULT hr = IAudioClient_Initialize(ac, AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED | AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE, dur, 0, mix, &GH_SESSION);
    if (FAILED(hr)) { L("[audio] Initialize failed 0x%08lX", hr); goto done; }
    if (FAILED(IAudioClient_GetService(ac, &IID_IAudioRenderClient, (void **)&rc))) goto done;
    g_ourRender = rc;
    if (SUCCEEDED(IAudioClient_GetService(ac, &IID_ISimpleAudioVolume, (void **)&sv)) && sv) {
        float v = 1.f; BOOL m = FALSE;
        ISimpleAudioVolume_GetMasterVolume(sv, &v); ISimpleAudioVolume_GetMute(sv, &m);
        if (v < 0.999f || m) L("[audio] haptics session was at %.0f%%%s, reset to 100%%", v * 100, m ? " and muted" : "");
        ISimpleAudioVolume_SetMasterVolume(sv, 1.f, NULL); ISimpleAudioVolume_SetMute(sv, FALSE, NULL);
    }
    UINT32 bufFrames = 0; IAudioClient_GetBufferSize(ac, &bufFrames);
    Synth S = {0};
    S.evR = g_evW;                      /* 0.2.4: start at the queue head, a fresh Synth used to stall on old slots */
    static int chimed;                  /* chime only on the first stream, not on every rebuild */
    if (C.chime > 0 && !chimed) { S.chime = (int)(sr * 0.12f); chimed = 1; }
    BYTE *p;
    if (SUCCEEDED(IAudioRenderClient_GetBuffer(rc, bufFrames, &p))) { synth_fill(&S, p, bufFrames, ch, isFloat, bits, iL, iR, sr); IAudioRenderClient_ReleaseBuffer(rc, bufFrames, 0); }
    if (FAILED(IAudioClient_Start(ac))) { L("[audio] Start failed"); goto done; }
    L("[audio] stream started (buffer %u frames), waiting for the device to play it", bufFrames);
    ok = 1;
    /* 0.2.4: the game's rumble is only replaced while the device is really pulling our audio.
     * If the stream stops moving, the endpoint changes or the session gets muted, haptics go
     * off (the game's own rumble comes back) and the stream is rebuilt. */
    ULONGLONG lastMove = GetTickCount64(), lastVol = lastMove;
    UINT32 lastPad = bufFrames;
    for (;;) {
        UINT32 pad = 0;
        hr = IAudioClient_GetCurrentPadding(ac, &pad);
        if (FAILED(hr)) { L("[audio] stream lost 0x%08lX, rebuilding", hr); break; }
        ULONGLONG now = GetTickCount64();
        if (pad < lastPad) {
            lastMove = now;
            if (!g_hapticsLive) { InterlockedExchange(&g_hapticsLive, 1); L("[audio] haptics streaming"); }
        }
        if (now - lastMove > 750) { L("[audio] stream stalled (device stopped playing it), rebuilding"); break; }
        if (g_rebuild) { L("[audio] controller endpoint changed, rebuilding"); break; }
        if (sv && now - lastVol > 2000) {
            lastVol = now; float v = 1.f; BOOL m = FALSE;
            if (SUCCEEDED(ISimpleAudioVolume_GetMasterVolume(sv, &v)) && SUCCEEDED(ISimpleAudioVolume_GetMute(sv, &m)) && (v < 0.999f || m)) {
                L("[audio] haptics session was turned down to %.0f%%%s, reset to 100%%", v * 100, m ? " and muted" : "");
                ISimpleAudioVolume_SetMasterVolume(sv, 1.f, NULL); ISimpleAudioVolume_SetMute(sv, FALSE, NULL);
            }
        }
        UINT32 avail = bufFrames - pad;
        if (avail >= 64) {
            if (FAILED(IAudioRenderClient_GetBuffer(rc, avail, &p))) { L("[audio] GetBuffer failed, rebuilding"); break; }
            synth_fill(&S, p, avail, ch, isFloat, bits, iL, iR, sr);
            IAudioRenderClient_ReleaseBuffer(rc, avail, 0);
            pad += avail;
        }
        lastPad = pad;
        Sleep(4);
    }
    InterlockedExchange(&g_hapticsLive, 0);
    L("[audio] haptics off, game rumble passes through until the stream is back");
    IAudioClient_Stop(ac);
done:
    if (sv) ISimpleAudioVolume_Release(sv);
    if (rc) IAudioRenderClient_Release(rc);
    if (mix) CoTaskMemFree(mix);
    if (ac) IAudioClient_Release(ac);
    g_ourClient = NULL; g_ourRender = NULL;
    if (epId) CoTaskMemFree(epId);
    IMMDevice_Release(dev);
    return ok;
}

static DWORD WINAPI audio_thread(void *arg)
{
    (void)arg;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    IMMDeviceEnumerator *en = NULL;
    if (FAILED(CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&en))) { L("[audio] no device enumerator"); return 0; }
    if (C.audioTap) install_audio_hooks(en);
    int warned = 0;
    for (;;) {
        int ok = run_stream(en);
        if (!ok && !warned) { warned = 1; L("[audio] DualSense audio endpoint not found yet. Plug the controller in by USB. Native rumble stays on until then."); }
        Sleep(ok ? 500 : 3000);
    }
}

/* 0.2.4 watchdog, low priority: learns which controller the game opened and asks the
 * stream to move when a better matching endpoint shows up or ours goes away. */
static DWORD WINAPI watch_thread(void *arg)
{
    (void)arg;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IMMDeviceEnumerator *en = NULL;
    if (FAILED(CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&en))) return 0;
    LONG seenSeq = 0;
    for (;;) {
        Sleep(2000);
        LONG seq = g_dsPathSeq;
        if (seq != seenSeq) {
            seenSeq = seq; wchar_t path[512];
            AcquireSRWLockShared(&g_dsPathLock); wcscpy(path, g_dsPath); ReleaseSRWLockShared(&g_dsPathLock);
            GUID c; t_inHook++; int got = hid_container(path, &c); t_inHook--;
            if (got) { g_dsContainer = c; InterlockedExchange(&g_dsContainerOk, 1); L("[watch] game's controller container {%08lX-...}", c.Data1); }
            else L("[watch] could not read the container of the game's controller");
        }
        wchar_t cur[512];
        AcquireSRWLockShared(&g_curEpLock); wcscpy(cur, g_curEp); ReleaseSRWLockShared(&g_curEpLock);
        if (!cur[0]) continue;
        wchar_t nm[256], *id = NULL;
        t_inHook++;
        IMMDevice *d = find_ds_endpoint(en, nm, &id, 1);
        t_inHook--;
        if (d) {
            if (id && wcscmp(id, cur)) InterlockedExchange(&g_rebuild, 1);
            IMMDevice_Release(d);
        } else InterlockedExchange(&g_rebuild, 1);
        if (id) CoTaskMemFree(id);
    }
}

static DWORD WINAPI ak_thread(void *arg)
{
    (void)arg;
    for (;;) { Sleep(2000); ak_report(); }
}

static DWORD WINAPI stats_thread(void *arg)
{
    (void)arg;
    LONG lw = -1, li = -1;
    for (int t = 0;; t++) {
        Sleep(5000);
        if (s_writes != lw || s_ioctl != li || s_giRumble || t % 12 == 0) {
            L("[stats] writes=%ld ioctl=%ld hidRumble=%ld giRumble=%ld giNonZero=%ld giRaw=%ld xi=%ld xiNonZero=%ld stripped=%ld bt=%ld maxLow=%ld maxHigh=%ld haptics=%s",
              s_writes, s_ioctl, s_rumble, s_giRumble, s_giRumbleNZ, s_giRaw, s_xi, s_xiNZ, s_stripped, s_bt, s_maxLow, s_maxHigh, g_hapticsLive ? "on" : "off");
            if (g_evN) L("[stats] sound event haptics fired: %ld", g_evFired);
            lw = s_writes; li = s_ioctl;
        }
        tap_stats();
        cen_report();
        if (t == 6 && s_writes == 0 && s_ioctl == 0)
            L("[stats] no DualSense output reports seen after 35 s. The game may use another path (GameInput). Send this log.");
    }
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID res)
{
    (void)res;
    if (reason != DLL_PROCESS_ATTACH) return TRUE;
    DisableThreadLibraryCalls(inst);
    load_real_winmm();
    GetModuleFileNameW(inst, g_dir, MAX_PATH);
    wchar_t *sl = wcsrchr(g_dir, L'\\'); if (sl) sl[1] = 0;
    wchar_t logp[MAX_PATH]; wcscpy(logp, g_dir); wcscat(logp, L"GearsHaptics.log");
    wcscpy(g_ini, g_dir); wcscat(g_ini, L"GearsHaptics.ini");
    {   /* carry settings over from the lantern builds */
        static const wchar_t *olds[] = { L"gears_haptics.ini", L"lambent.ini", L"lantern.ini" };
        for (int i = 0; i < 3; i++) {
            wchar_t old[MAX_PATH]; wcscpy(old, g_dir); wcscat(old, olds[i]);
            if (GetFileAttributesW(g_ini) == INVALID_FILE_ATTRIBUTES && GetFileAttributesW(old) != INVALID_FILE_ATTRIBUTES) MoveFileW(old, g_ini);
        }
    }
    InitializeCriticalSection(&g_logCs);
    g_log = _wfopen(logp, L"w");
    wchar_t exe[MAX_PATH]; GetModuleFileNameW(NULL, exe, MAX_PATH);
    L("Gears Haptics %s starting in %ls", LANTERN_VERSION, exe);
    L("real winmm: %p", g_realWinmm);
    load_config();
    install_hooks();
    CloseHandle(CreateThread(NULL, 0, audio_thread, NULL, 0, NULL));
    CloseHandle(CreateThread(NULL, 0, stats_thread, NULL, 0, NULL));
    CloseHandle(CreateThread(NULL, 0, ak_thread, NULL, 0, NULL));
    CloseHandle(CreateThread(NULL, 0, gi_thread, NULL, 0, NULL));
    CloseHandle(CreateThread(NULL, 0, watch_thread, NULL, 0, NULL));
    return TRUE;
}
