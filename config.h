#ifndef CONFIG_H
#define CONFIG_H

// ----- misc definitions -----
#define MOD           Mod4Mask
#define CORNER_RADIUS 20

// ----- command -----
const char *lock[]    = { "slock",                       0 };
const char *menu[]    = { "ndmen",                       0 };
const char *term[]    = { "xterm", "-title", "terminal", 0 };
const char *scrot[]   = { "scr",                         0 };
const char *donil[]   = { "echo",                        0 };  // do-nothing command

// ------ keybindings ------
static struct key keys[] = {

	// modifiers    keys       function    args
	{MOD|ShiftMask, XK_e,      wm_exit,  {0}},
	{MOD|ShiftMask, XK_q,      win_kill,   {0}},
	{MOD,           XK_c,      win_center, {0}},
	{MOD,           XK_f,      win_fs,     {0}},
	{MOD,           XK_j,      win_next,   {0}},
	{MOD,           XK_k,      win_prev,   {0}},

	{MOD,           XK_y,      win_modify, {.i = RESIZE_LEFT}},
	{MOD,           XK_u,      win_modify, {.i = RESIZE_DOWN}},
	{MOD,           XK_i,      win_modify, {.i = RESIZE_UP}},
	{MOD,           XK_o,      win_modify, {.i = RESIZE_RIGHT}},

	{MOD|ShiftMask, XK_h,      win_modify, {.i = MOVE_LEFT}},
	{MOD|ShiftMask, XK_j,      win_modify, {.i = MOVE_DOWN}},
	{MOD|ShiftMask, XK_k,      win_modify, {.i = MOVE_UP}},
	{MOD|ShiftMask, XK_l,      win_modify, {.i = MOVE_RIGHT}},

	{MOD|ShiftMask, XK_s,      run,        {.com = lock}},
	{MOD,           XK_p,      run,        {.com = menu}},
	{MOD|ShiftMask, XK_p,      run,        {.com = scrot}},
	{MOD,           XK_Return, run,        {.com = term}},

	{0,             XF86XK_AudioLowerVolume,  run, {.com = donil}},
	{0,             XF86XK_AudioRaiseVolume,  run, {.com = donil}},
	{0,             XF86XK_AudioMute,         run, {.com = donil}},
	{0,             XF86XK_MonBrightnessUp,   run, {.com = donil}},
	{0,             XF86XK_MonBrightnessDown, run, {.com = donil}},

	{MOD,           XK_1,      ws_go,      {.i = 1}},
	{MOD|ShiftMask, XK_1,      win_to_ws,  {.i = 1}},
	{MOD,           XK_2,      ws_go,      {.i = 2}},
	{MOD|ShiftMask, XK_2,      win_to_ws,  {.i = 2}},
	{MOD,           XK_3,      ws_go,      {.i = 3}},
	{MOD|ShiftMask, XK_3,      win_to_ws,  {.i = 3}},
	{MOD,           XK_4,      ws_go,      {.i = 4}},
	{MOD|ShiftMask, XK_4,      win_to_ws,  {.i = 4}},
	{MOD,           XK_5,      ws_go,      {.i = 5}},
	{MOD|ShiftMask, XK_5,      win_to_ws,  {.i = 5}},
	{MOD,           XK_6,      ws_go,      {.i = 6}},
	{MOD|ShiftMask, XK_6,      win_to_ws,  {.i = 6}},
};

#endif
