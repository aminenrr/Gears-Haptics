/* Writes the default GearsHaptics.ini and GearsHaptics_events.ini (the same text the DLL creates) into out/. */
#include <stdio.h>
#include <string.h>
#include "../src/defaults_ini.h"
#include "../src/events_default.h"

static int put(const char *path, const char *a, const char *b)
{
    FILE *f = fopen(path, "wb");
    if (!f) { perror(path); return 1; }
    fwrite(a, 1, strlen(a), f);
    if (b) { fwrite("\r\n", 1, 2, f); fwrite(b, 1, strlen(b), f); }
    fclose(f);
    return 0;
}

int main(void)
{
    return put("out/GearsHaptics.ini", DEFAULT_INI, DEFAULT_SOUND_INI)
         | put("out/GearsHaptics_events.ini", DEFAULT_EVENTS_INI, NULL);
}
