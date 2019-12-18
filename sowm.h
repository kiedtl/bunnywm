#ifndef SOWM_H
#define SOWM_H

#include "types.h"
#include <xcb/xcb.h>
#include <stdio.h>
#include <xcb/xcb_ewmh.h>
#include <X11/Xlib.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define MAX_WS	32

typedef union {
    const char** com;
    const int i;
    const Window w;
} Arg;

typedef struct key {
    unsigned int mod;
    KeySym keysym;
    void (*function)(const Arg arg);
    const Arg arg;
} key;

typedef struct client {
    struct client *next, *prev;
    i32 f, wx, wy;
    usize ww, wh;
    Window w;
} client;

int xerror(void);
void sowm_exit(void);
void win_focus ( client *c );
void notify_destroy(XEvent *e);
void notify_enter(XEvent *e);
void notify_motion(XEvent *e);
void key_press(XEvent *e);
void button_press(XEvent *e);
void button_release(void);
void win_add(Window w);
void win_del(Window w);
void win_kill(void);
void win_center(void);
void win_fs(void);
void win_to_ws(const Arg arg);
void win_prev( void );
void win_next( void );
void win_modify ( const Arg arg );
void ws_go(const Arg arg);
void configure_request(XEvent *e);
void map_request(XEvent *e);
void run(const Arg arg);

extern xcb_ewmh_connection_t *junk;

client       *list = {0}, *ws_list[10] = {0}, *cur;
isize        ws = 1, sw, sh, wx, wy;
usize        ww, wh;

Display      *d;
XButtonEvent mouse;

// new xcb stuff (WIP)
xcb_connection_t       *con;
const xcb_setup_t      *setup;
xcb_screen_iterator_t  it;
xcb_screen_t           *screen;

void (*events[LASTEvent])(XEvent *e) = {
    [ButtonPress]      = button_press,
    [ButtonRelease]    = button_release,
    [ConfigureRequest] = configure_request,
    [KeyPress]         = key_press,
    [MapRequest]       = map_request,
    [DestroyNotify]    = notify_destroy,
    [EnterNotify]      = notify_enter,
    [MotionNotify]     = notify_motion
};

#define EPRINT(...) fprintf(stderr, __VA_ARGS__);
#define win         (client *t=0, *c=list; c && t!=list->prev; t=c, c=c->next)
#define ws_save(W)  ws_list[W] = list
#define ws_sel(W)   list = ws_list[ws = W]

#define win_size(W, gx, gy, gw, gh) \
    XGetGeometry(d, W, &(Window){0}, gx, gy, gw, gh, \
                 &(unsigned int){0}, &(unsigned int){0})

#endif
