#ifndef CONFIG_H
#define CONFIG_H

#define MOD Mod4Mask

const char* menu[]    = {"ndmen",      0};
const char* term[]    = {"xterm", "-title terminal", 0};
const char* scrot[]   = {"scr",            0};

static struct key keys[] = {
    {MOD|ShiftMask, XK_q,   win_kill,   {0}},
    {MOD,           XK_c,   win_center, {0}},
    {MOD,           XK_f,   win_fs,     {0}},
    {MOD,           XK_j,   win_next,   {0}},
    {MOD,           XK_k,   win_prev,   {0}},
    //{Mod1Mask,     XK_Tab, win_next,   {0}},

    {MOD,           XK_p,      run, {.com = menu}},
    {MOD|ShiftMask, XK_p,      run, {.com = scrot}},
    {MOD,           XK_Return, run, {.com = term}},

    //{0,   XF86XK_AudioLowerVolume,  run, {.com = voldown}},
    //{0,   XF86XK_AudioRaiseVolume,  run, {.com = volup}},
    //{0,   XF86XK_AudioMute,         run, {.com = volmute}},
    //{0,   XF86XK_MonBrightnessUp,   run, {.com = briup}},
    //{0,   XF86XK_MonBrightnessDown, run, {.com = bridown}},

    {MOD,           XK_1, ws_go,     {.i = 1}},
    {MOD|ShiftMask, XK_1, win_to_ws, {.i = 1}},
    {MOD,           XK_2, ws_go,     {.i = 2}},
    {MOD|ShiftMask, XK_2, win_to_ws, {.i = 2}},
    {MOD,           XK_3, ws_go,     {.i = 3}},
    {MOD|ShiftMask, XK_3, win_to_ws, {.i = 3}},
    {MOD,           XK_4, ws_go,     {.i = 4}},
    {MOD|ShiftMask, XK_4, win_to_ws, {.i = 4}},
    {MOD,           XK_5, ws_go,     {.i = 5}},
    {MOD|ShiftMask, XK_5, win_to_ws, {.i = 5}},
    {MOD,           XK_6, ws_go,     {.i = 6}},
    {MOD|ShiftMask, XK_6, win_to_ws, {.i = 6}},
};

#endif
