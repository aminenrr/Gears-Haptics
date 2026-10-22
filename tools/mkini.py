# Writes the default GearsHaptics.ini and GearsHaptics_events.ini (the same text the DLL creates) into out/.
import re, ast, os
def cstr(src, name):
    m = re.search(r'static const char \*' + name + r' =\s*(.*?);\s*\n', src, re.S)
    return ''.join(ast.literal_eval(x) for x in re.findall(r'"(?:[^"\\]|\\.)*"', m.group(1)))
main = open('src/main.c', encoding='utf-8').read()
ev = open('src/events_default.h', encoding='utf-8').read()
os.makedirs('out', exist_ok=True)
open('out/GearsHaptics.ini', 'w', newline='').write(cstr(main, 'DEFAULT_INI') + '\r\n' + cstr(main, 'DEFAULT_SOUND_INI'))
open('out/GearsHaptics_events.ini', 'w', newline='').write(cstr(ev, 'DEFAULT_EVENTS_INI'))
