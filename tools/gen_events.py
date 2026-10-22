# Builds the default sound haptics allowlist from a lantern_events.txt catalog.
# Player events in E-Day carry no suffix (squad _SQUD, enemies _ENMY, cutscenes _CINE, shared _SHRD...).
import re, sys
cat = {}
for ln in open(sys.argv[1], encoding='utf-8', errors='replace'):
    if ln.startswith('#'): continue
    p = ln.rstrip('\n').split('\t')
    if len(p) == 3 and p[2] != '?': cat[p[2]] = int(p[0])
def fnv(s):
    h = 2166136261
    for c in s.lower().encode(): h = ((h * 16777619) & 0xFFFFFFFF) ^ c
    return h
names = {n for n, i in cat.items() if fnv(n) == i}
# drop names that are another name plus one stray byte from memory
names = {n for n in names if not (n[:-1] in names)}
BAD = re.compile(r'_(SQUD|ENMY|CINE|ByRole|NPI|PIS|VCTM|SHRD|PLYR_Remote|2D_Remote|MP)\b|_(SQUD|ENMY|CINE|ByRole|NPI|PIS|VCTM|SHRD)')
HEAVY = 'Boomshot Breechshot Longshot Gnasher Dropshot Mulcher Torque Markza Embar GrenadeLauncher GutPuncher Gutpuncher Overkill Retro Sawedoff SawedOff Boltok Trishot CluckPuncher Buzzkill Hammer_of_Dawn HammerOfDawn Salvo'.split()
RIFLE = 'Lancer ProtoLancer Hammerburst HammerBurst Claw Enforcer Lancer_MK1 LancerGL'.split()
rules = []
def R(pat, preset, gain, side='B'): rules.append((re.compile(pat), preset, gain, side))
# weapons
R(r'^Play_WEP_(\w+?)_Fire(_FirstShot)?$', 'auto', 0, 'B')
R(r'^Play_WEP_\w+_Fire_(Empty|ClipEndReached|NoAmmoReached)$', 'tick', 0.4)
R(r'^Play_WEP_\w+_Reload_(ClipInsert|CockClose|BoltClose|Hit|Arm|Insert)$', 'knock', 0.6)
R(r'^Play_WEP_\w+_Reload_(ClipEject|CockOpen|BoltOpen|Open)$', 'click', 0.5)
R(r'^Play_WEP_\w+_Reload_(ClipEject_Impact|AmmoGrab)$', 'tick', 0.3)
R(r'^Play_WEP_\w+_Reload_PerfectReload$', 'buzz', 0.6)
R(r'^Play_WEP_\w+_Reload_Jammed$', 'knock', 0.8)
R(r'^Play_WEP_\w+_(Equip|Unequip)$', 'tick', 0.35)
R(r'^Play_WEP_GrenadeFrag_Bolo_Throw$', 'knock', 0.6)
R(r'^Play_WEP_GrenadeFrag_Bolo_Swing$', 'buzz', 0.35)
R(r'^Play_WEP_\w*Chainsaw_(Start|Rev\w*|Slash\w*)$', 'buzz', 0.7)
R(r'^Play_WEP_Kick_Melee_PCI$', 'knock', 0.7)
R(r'^Play_IMP_\w*MeleeImpact_PCI$', 'thump', 0.9)
R(r'^Play_Gore_\w*ByPlayer$', 'thump', 1.0)
# body and boots (player only)
R(r'^Play_FS_Boot_COGCombat_Step$', 'thump', 0.35, 'A')
R(r'^Play_FS_Boot_COGCombat_Land$', 'thump', 0.8)
R(r'^Play_FS_Boot_COGCombat_Leap$', 'knock', 0.4)
R(r'^Play_FS_Boot_COGCombat_(Scuff|Ladder)$', 'tick', 0.25)
R(r'^Play_BF_Humanoid_FootScuff_RoadieSprint_Sweetener$', 'knock', 0.35, 'A')
R(r'^Play_BF_Mantle_Enter_Sweetener_PLYR$', 'knock', 0.6)
R(r'^Play_EFF_Marcus_CoverSlam$', 'thump', 0.8)
R(r'^Play_EFF_Marcus_CoverEnterSoft$', 'knock', 0.4)
R(r'^Play_EFF_Marcus_Crouch$', 'tick', 0.3)
R(r'^Play_BI_HandMediumGlove_Grab$', 'knock', 0.35)
R(r'^Play_BI_HandMediumGlove_Touch$', 'tick', 0.25)
R(r'^Play_Ability_SpecialGear_\w+$', 'buzz', 0.5)
# world
R(r'^Play_INT_\w+_(Open|Close|Smash)$', 'knock', 0.6)
R(r'^Play_INT_Door_Metal_Smash$', 'thump', 1.0)
R(r'^Play_SP_ScreenShake_Impacts_\w+$', 'thump', 1.0)
R(r'^Play_SP_ScreenShake_Rumble_\w+_Start$', 'thump', 0.6)
R(r'^Play_SP_Ehole_(Bump|Crack)$', 'thump', 0.7)
R(r'^Play_SP_Ehole_Open$', 'thump', 1.0)
R(r'^Play_WEP_GrenadeFrag_Explosion_SHRD$', 'thump', 0.8)
R(r'^Play_IMP_2D_HitNotify_Default$', 'tick', 0.35)
# UI
R(r'^Play_UI_\w*Navigat\w*(UpDown|Sideways)$', 'tick', 0.35)
R(r'^Play_UI_MainMenu_Navigation_Select$', 'click', 0.6)
R(r'^Play_UI_MainMenu_Navigation_Back$', 'click', 0.4)
R(r'^Play_UI_MainMenu_PressAtoStart$', 'click', 0.6)
R(r'^Play_UI_\w+_(Open|Close)$', 'click', 0.4)
R(r'^Play_UI_Forge_Item_Equip$', 'knock', 0.5)
R(r'^Play_UI_Campaign_Objective(Updated|Completed)$', 'buzz', 0.4)
R(r'^Play_UI_Reload_Active$', 'tick', 0.5)
R(r'^Play_UI_Reload_Perfect$', 'buzz', 0.8)
R(r'^Play_UI_Reload_Fail$', 'thump', 0.7)
R(r'^Play_UI_LowHealth_Warning$', 'thump', 0.6)
R(r'^Global_Pause_(Start|Release)$', 'click', 0.4)
out = []
for n in sorted(names):
    if BAD.search(n) and not n.endswith('_PLYR') and not n.endswith('_PCI') and n != 'Play_WEP_GrenadeFrag_Explosion_SHRD': continue
    for rx, pr, g, sd in rules:
        m = rx.match(n)
        if not m: continue
        if pr == 'auto':
            w = m.group(1)
            if any(w.startswith(h) for h in HEAVY): pr, g = 'thump', 0.9
            elif any(w.startswith(r) for r in RIFLE): pr, g = 'click', 0.45
            else: pr, g = 'knock', 0.7
        out.append((n, cat[n], pr, g, sd)); break
print('; lantern sound event haptics. One line per event: id=preset,gain,side  ; name')
print('; presets: tick click knock thump buzz. side: B both, L, R, A alternate (boots)')
print('; comment a line with ; to turn that event off. Delete this file to get the defaults back.')
print('[Events]')
sec = None
for n, i, pr, g, sd in out:
    s = n.split('_')[1] if n.startswith('Play_') else n.split('_')[0]
    if s != sec: print(f'; ---- {s}'); sec = s
    print(f'{i}={pr},{g:g},{sd} ; {n}')
print(f'; {len(out)} events', file=sys.stderr)
