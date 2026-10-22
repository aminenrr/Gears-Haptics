/*
 * lantern: winmm.dll proxy.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "MinHook.h"
#include "winmm_names.h"

#define LANTERN_VERSION "0.1.0-p1"

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
    int enabled, keepNative, logReports;
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

static const char *DEFAULT_INI =
    "; lantern settings. Delete this file to get the defaults back.\r\n"
    "[General]\r\n"
    "Enabled=1\r\n"
    "; master haptic strength 0..2\r\n"
    "Strength=1.0\r\n"
    "; 1 = also keep the game's own rumble (not recommended)\r\n"
    "KeepNativeRumble=0\r\n"
    "; how many raw DualSense reports to write to the log (for debugging)\r\n"
    "LogReports=40\r\n"
    "; short pulse when haptics connect (0 = off)\r\n"
    "Chime=1\r\n"
    "\r\n[Tuning]\r\n"
    "; heavy motor = deep thump, light motor = sharp buzz\r\n"
    "LowMotorHz=75\r\n"
    "HighMotorHz=185\r\n"
    "LowGain=1.0\r\n"
    "HighGain=0.8\r\n"
    "; heavy plays left, light plays right; how much leaks to the other side (0..1)\r\n"
    "SideBleed=0.35\r\n"
    "; extra click on sudden hits (0 = off)\r\n"
    "TransientGain=0.9\r\n"
    "TransientThreshold=0.2\r\n";

static void load_config(void)
{
    if (GetFileAttributesW(g_ini) == INVALID_FILE_ATTRIBUTES) {
        HANDLE h = CreateFileW(g_ini, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, DEFAULT_INI, (DWORD)strlen(DEFAULT_INI), &w, NULL); CloseHandle(h); }
    }
    C.enabled    = (int)ini_f(L"General", L"Enabled", 1);
    C.strength   = ini_f(L"General", L"Strength", 1.0f);
    C.keepNative = (int)ini_f(L"General", L"KeepNativeRumble", 0);
    C.logReports = (int)ini_f(L"General", L"LogReports", 40);
    C.chime      = ini_f(L"General", L"Chime", 1);
    C.lowHz      = ini_f(L"Tuning", L"LowMotorHz", 75);
    C.highHz     = ini_f(L"Tuning", L"HighMotorHz", 185);
    C.lowGain    = ini_f(L"Tuning", L"LowGain", 1.0f);
    C.highGain   = ini_f(L"Tuning", L"HighGain", 0.8f);
    C.bleed      = ini_f(L"Tuning", L"SideBleed", 0.35f);
    C.trGain     = ini_f(L"Tuning", L"TransientGain", 0.9f);
    C.trThresh   = ini_f(L"Tuning", L"TransientThreshold", 0.2f);
    L("config: Enabled=%d Strength=%.2f KeepNativeRumble=%d LowHz=%.0f HighHz=%.0f LowGain=%.2f HighGain=%.2f Bleed=%.2f TrGain=%.2f TrThr=%.2f",
      C.enabled, C.strength, C.keepNative, C.lowHz, C.highHz, C.lowGain, C.highGain, C.bleed, C.trGain, C.trThresh);
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
    char s[3 * 24 + 1]; int k = 0;
    for (DWORD i = 0; i < n && i < 24; i++) k += sprintf(s + k, "%02X ", b[i]);
    s[k] = 0;
    L("%s len=%lu: %s", tag, n, s);
}

static void max_store(volatile LONG *m, LONG v) { LONG o; while ((o = *m) < v && InterlockedCompareExchange(m, v, o) != o) {} }

/* b is the game's own buffer; motor bytes are edited in place. */
static void process_report(BYTE *b, DWORD n, const char *via)
{
    if (s_logged < C.logReports) { InterlockedIncrement(&s_logged); hexlog(via, b, n); }
    if (n < 1) return;
    if (b[0] == 0x31) {                         /* Bluetooth report, haptics need USB */
        if (InterlockedIncrement(&s_bt) == 1) L("[hid] Bluetooth output reports seen. Haptics only work over USB: unplug Bluetooth, use the cable.");
        return;
    }
    if (b[0] != 0x02 || n < 48) return;
    BYTE f0 = b[1];
    if (f0 & 0x03) {
        BYTE hi = b[3], lo = b[4];             /* [3] right/light motor, [4] left/heavy motor */
        g_high = hi / 255.0f; g_low = lo / 255.0f;
        if (hi || lo) { InterlockedIncrement(&s_rumble); max_store(&s_maxLow, lo); max_store(&s_maxHigh, hi); }
        if (C.enabled && g_hapticsLive && !C.keepNative) {
            b[1] = (BYTE)(f0 & ~0x03);          /* drop rumble emulation, keep trigger/light flags */
            b[3] = 0; b[4] = 0;
            InterlockedIncrement(&s_stripped);
        }
    }
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
        if (is_ds_path_w(p)) { remember(h, 1); L("[hid] game opened DualSense: %ls (access 0x%lX, flags 0x%lX) -> %p", p, a, f, h); }
    }
    return h;
}

static HANDLE WINAPI hCreateFileA(LPCSTR p, DWORD a, DWORD s, LPSECURITY_ATTRIBUTES sa, DWORD d, DWORD f, HANDLE t)
{
    HANDLE h = oCreateFileA(p, a, s, sa, d, f, t);
    if (h != INVALID_HANDLE_VALUE && !t_inHook && p && p[0] == '\\' && p[1] == '\\') {
        wchar_t w[512]; MultiByteToWideChar(CP_ACP, 0, p, -1, w, 512);
        if (is_ds_path_w(w)) { remember(h, 1); L("[hid] game opened DualSense (A): %s -> %p", p, h); }
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

static IMMDevice *find_ds_endpoint(IMMDeviceEnumerator *en, wchar_t *nameOut)
{
    IMMDeviceCollection *col = NULL; IMMDevice *found = NULL;
    if (FAILED(IMMDeviceEnumerator_EnumAudioEndpoints(en, eRender, DEVICE_STATE_ACTIVE, &col))) return NULL;
    UINT n = 0; IMMDeviceCollection_GetCount(col, &n);
    for (UINT i = 0; i < n && !found; i++) {
        IMMDevice *d = NULL; if (FAILED(IMMDeviceCollection_Item(col, i, &d))) continue;
        IPropertyStore *ps = NULL;
        if (SUCCEEDED(IMMDevice_OpenPropertyStore(d, STGM_READ, &ps))) {
            PROPVARIANT v; PropVariantInit(&v);
            if (SUCCEEDED(IPropertyStore_GetValue(ps, &PKEY_Device_FriendlyName, &v)) && v.vt == VT_LPWSTR) {
                if (wcsstr(v.pwszVal, L"Wireless Controller")) { found = d; wcsncpy(nameOut, v.pwszVal, 255); nameOut[255] = 0; }
            }
            PropVariantClear(&v);
            IPropertyStore_Release(ps);
        }
        if (!found) IMMDevice_Release(d);
    }
    IMMDeviceCollection_Release(col);
    return found;
}

typedef struct { double phL, phH, phT; float low, high, tr, lastSum; int chime; } Synth;

static void synth_fill(Synth *S, BYTE *out, UINT frames, int ch, int isFloat, int bits, int iL, int iR, int sr)
{
    float str = C.enabled ? C.strength : 0.f;
    float tl = g_low, th = g_high;
    float sum = tl + th;
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
        if (S->chime > 0) { sT += (float)sin(S->phH * 1.3) * 0.5f; S->chime--; }
        float left = tanhf((sL + sH * C.bleed + sT) * str * 1.4f);
        float right = tanhf((sH + sL * C.bleed + sT) * str * 1.4f);
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
    wchar_t name[256] = L"";
    IMMDevice *dev = find_ds_endpoint(en, name);
    if (!dev) return 0;
    IAudioClient *ac = NULL; IAudioRenderClient *rc = NULL; WAVEFORMATEX *mix = NULL; int ok = 0;
    if (FAILED(IMMDevice_Activate(dev, &IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&ac))) goto done;
    if (FAILED(IAudioClient_GetMixFormat(ac, &mix))) goto done;
    int ch = mix->nChannels, sr = mix->nSamplesPerSec, bits = mix->wBitsPerSample, isFloat = 0;
    DWORD mask = 0;
    if (mix->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        WAVEFORMATEXTENSIBLE *x = (WAVEFORMATEXTENSIBLE *)mix;
        mask = x->dwChannelMask;
        isFloat = IsEqualGUID(&x->SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
    } else if (mix->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) isFloat = 1;
    L("[audio] DualSense endpoint: %ls (%d Hz, %d ch, %d bit, %s, mask 0x%lX)", name, sr, ch, bits, isFloat ? "float" : "pcm", mask);
    if (ch < 4) { L("[audio] endpoint has %d channels, need 4 (is the controller on USB?)", ch); goto done; }
    if (popcount(mask) != ch) mask = (ch == 4) ? (0x1 | 0x2 | SPK_BL | SPK_BR) : 0;
    int iL = chan_index(mask, SPK_BL, SPK_SL, 2), iR = chan_index(mask, SPK_BR, SPK_SR, 3);
    if (iL >= ch || iR >= ch) { iL = 2; iR = 3; }
    L("[audio] haptic channels: left actuator ch%d, right actuator ch%d", iL + 1, iR + 1);

    REFERENCE_TIME dur = 300000; /* 30 ms */
    HRESULT hr = IAudioClient_Initialize(ac, AUDCLNT_SHAREMODE_SHARED, 0, dur, 0, mix, NULL);
    if (FAILED(hr)) { L("[audio] Initialize failed 0x%08lX", hr); goto done; }
    if (FAILED(IAudioClient_GetService(ac, &IID_IAudioRenderClient, (void **)&rc))) goto done;
    UINT32 bufFrames = 0; IAudioClient_GetBufferSize(ac, &bufFrames);
    Synth S = {0};
    if (C.chime > 0) S.chime = (int)(sr * 0.12f);
    BYTE *p;
    if (SUCCEEDED(IAudioRenderClient_GetBuffer(rc, bufFrames, &p))) { synth_fill(&S, p, bufFrames, ch, isFloat, bits, iL, iR, sr); IAudioRenderClient_ReleaseBuffer(rc, bufFrames, 0); }
    IAudioClient_Start(ac);
    InterlockedExchange(&g_hapticsLive, 1);
    L("[audio] haptics streaming (buffer %u frames)", bufFrames);
    ok = 1;
    for (;;) {
        UINT32 pad = 0;
        hr = IAudioClient_GetCurrentPadding(ac, &pad);
        if (FAILED(hr)) { L("[audio] stream lost 0x%08lX, retrying", hr); break; }
        UINT32 avail = bufFrames - pad;
        if (avail >= 64) {
            if (FAILED(IAudioRenderClient_GetBuffer(rc, avail, &p))) break;
            synth_fill(&S, p, avail, ch, isFloat, bits, iL, iR, sr);
            IAudioRenderClient_ReleaseBuffer(rc, avail, 0);
        }
        Sleep(4);
    }
    InterlockedExchange(&g_hapticsLive, 0);
    IAudioClient_Stop(ac);
done:
    if (rc) IAudioRenderClient_Release(rc);
    if (mix) CoTaskMemFree(mix);
    if (ac) IAudioClient_Release(ac);
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
    int warned = 0;
    for (;;) {
        if (!run_stream(en) && !warned) { warned = 1; L("[audio] DualSense audio endpoint not found yet. Plug the controller in by USB. Native rumble stays on until then."); }
        Sleep(3000);
    }
}

static DWORD WINAPI stats_thread(void *arg)
{
    (void)arg;
    LONG lw = -1, li = -1;
    for (int t = 0;; t++) {
        Sleep(5000);
        if (s_writes != lw || s_ioctl != li || t % 12 == 0) {
            L("[stats] writes=%ld ioctl=%ld rumble=%ld stripped=%ld bt=%ld maxLow=%ld maxHigh=%ld haptics=%s",
              s_writes, s_ioctl, s_rumble, s_stripped, s_bt, s_maxLow, s_maxHigh, g_hapticsLive ? "on" : "off");
            lw = s_writes; li = s_ioctl;
        }
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
    wchar_t logp[MAX_PATH]; wcscpy(logp, g_dir); wcscat(logp, L"lantern.log");
    wcscpy(g_ini, g_dir); wcscat(g_ini, L"lantern.ini");
    InitializeCriticalSection(&g_logCs);
    g_log = _wfopen(logp, L"w");
    wchar_t exe[MAX_PATH]; GetModuleFileNameW(NULL, exe, MAX_PATH);
    L("lantern %s starting in %ls", LANTERN_VERSION, exe);
    L("real winmm: %p", g_realWinmm);
    load_config();
    install_hooks();
    CloseHandle(CreateThread(NULL, 0, audio_thread, NULL, 0, NULL));
    CloseHandle(CreateThread(NULL, 0, stats_thread, NULL, 0, NULL));
    return TRUE;
}
