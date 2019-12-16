#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define EPRINT(...)		fprintf(stderr, __VA_ARGS__);

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
    int f, wx, wy;
    unsigned int ww, wh;
    Window w;
} client;

static int xerror ( void );
static void button_press(XEvent *e);
static void button_release( void );
static void configure_request(XEvent *e);
static void key_press(XEvent *e);
static void map_request(XEvent *e);
static void notify_destroy(XEvent *e);
static void notify_enter(XEvent *e);
static void notify_motion(XEvent *e);
static void run(const Arg arg);
static void win_add(Window w);
static void win_center( void );
static void win_del(Window w);
static void win_fs( void );
static void win_kill( void );
static void win_prev( void );
static void win_next( void );
static void win_to_ws(const Arg arg);
static void ws_go(const Arg arg);
static void win_focus ( client *c );

static client       *list = {0}, *ws_list[10] = {0}, *cur;
static int          ws = 1, sw, sh, wx, wy;
static unsigned int ww, wh;

static Display      *d;
static XButtonEvent mouse;

static void (*events[LASTEvent])(XEvent *e) = {
    [ButtonPress]      = button_press,
    [ButtonRelease]    = button_release,
    [ConfigureRequest] = configure_request,
    [KeyPress]         = key_press,
    [MapRequest]       = map_request,
    [DestroyNotify]    = notify_destroy,
    [EnterNotify]      = notify_enter,
    [MotionNotify]     = notify_motion
};

#define win        (client *t=0, *c=list; c && t!=list->prev; t=c, c=c->next)
#define ws_save(W) ws_list[W] = list
#define ws_sel(W)  list = ws_list[ws = W]

#define win_size(W, gx, gy, gw, gh) \
    XGetGeometry(d, W, &(Window){0}, gx, gy, gw, gh, \
                 &(unsigned int){0}, &(unsigned int){0})
