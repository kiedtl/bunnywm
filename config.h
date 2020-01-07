#ifndef CONFIG_H
#define CONFIG_H

// ----- definitions -----
// only mess with this if you know
// what you're doing!
#define MOD Mod4Mask

#define RESIZE_LEFT		0x1
#define RESIZE_DOWN		0x2
#define RESIZE_UP		0x3
#define RESIZE_RIGHT		0x4
#define MOVE_LEFT		0x5
#define MOVE_DOWN		0x6
#define MOVE_UP			0x7
#define MOVE_RIGHT		0x8

// ----- command -----
const char *lock[]    = { "slock",                       0 };
const char *menu[]    = { "ndmen",                       0 };
const char *term[]    = { "xterm", "-title", "terminal", 0 };
const char *scrot[]   = { "scr",                         0 };

// ------ keybindings ------
static struct key keys[] = {
	{MOD|ShiftMask, XK_e,   sowm_exit,  {0}},
	{MOD|ShiftMask, XK_q,   win_kill,   {0}},
	{MOD,           XK_c,   win_center, {0}},
	{MOD,           XK_f,   win_fs,     {0}},
	{MOD,           XK_j,   win_next,   {0}},
	{MOD,           XK_k,   win_prev,   {0}},
	//{Mod1Mask,     XK_Tab, win_next,   {0}},
	
	{Mod1Mask,      XK_h,   win_modify, {.i = RESIZE_LEFT}},
	{Mod1Mask,      XK_j,   win_modify, {.i = RESIZE_DOWN}},
	{Mod1Mask,      XK_k,   win_modify, {.i = RESIZE_UP}},
	{Mod1Mask,      XK_l,   win_modify, {.i = RESIZE_RIGHT}},

	{MOD|ShiftMask, XK_h,   win_modify, {.i = MOVE_LEFT}},
	{MOD|ShiftMask, XK_j,   win_modify, {.i = MOVE_DOWN}},
	{MOD|ShiftMask, XK_k,   win_modify, {.i = MOVE_UP}},
	{MOD|ShiftMask, XK_l,   win_modify, {.i = MOVE_RIGHT}},

	{MOD|ShiftMask, XK_S,      run, {.com = lock}},
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
