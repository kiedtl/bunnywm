//
// kiedtl's sowm
//

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <X11/Xlib-xcb.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "sowm.h"
#include "config.h"

int
main ( void )
{
    XEvent ev;

    if (!(d = XOpenDisplay(0)))
	{
			EPRINT("sowm: error: unable to open X display.\n");
			exit(1);
	}

    signal(SIGCHLD, SIG_IGN);
    XSetErrorHandler(xerror);

	// setup xcb stuff (WIP)
	con = XGetXCBConnection(d);
	setup = xcb_get_setup(con);
	it = xcb_setup_roots_iterator(setup);
	screen = it.data;

    //int s       = DefaultScreen(d);
    Window root = screen->root; //RootWindow(d, s);

    sw          = screen->width_in_pixels; //XDisplayWidth(d, s);
	sh          = screen->height_in_pixels; // XDisplayHeight(d, s);

	XSelectInput(d,root,SubstructureRedirectMask);
    XDefineCursor(d, root, XCreateFontCursor(d, 68));

    for (usize i = 0; i < sizeof(keys)/sizeof(*keys); ++i)
        XGrabKey(d, XKeysymToKeycode(d, keys[i].keysym), keys[i].mod,
                 root, True, GrabModeAsync, GrabModeAsync);

    for (usize i = 1; i < 4; i += 2)
        XGrabButton(d, i, MOD, root, True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
            GrabModeAsync, GrabModeAsync, 0, 0);

    while (1 && !XNextEvent(d, &ev))
        if (events[ev.type]) events[ev.type](&ev);
}

static int 
xerror ( void )
{
		return 0;
}

void
win_focus ( client *c )
{
    if (c == NULL || !c->w) {
		//xcb_set_input_focus(con, XCB_NONE, XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
		return;
	}

    //xcb_set_input_focus(con, XCB_INPUT_FOCUS_PARENT, c->w, XCB_CURRENT_TIME);
	//xcb_flush(con);

    XSetInputFocus(d, c->w, RevertToParent, CurrentTime);
}

void
notify_destroy ( XEvent *e )
{
    win_del(e->xdestroywindow.window);

    if (list) win_focus(list->prev);
}

void
notify_enter ( XEvent *e )
{
    while(XCheckTypedEvent(d, EnterNotify, e));

    for win if (c->w == e->xcrossing.window) win_focus(c);
}

void
notify_motion ( XEvent *e )
{
    if (!mouse.subwindow || cur->f) return;

    while(XCheckTypedEvent(d, MotionNotify, e));

    int xd = e->xbutton.x_root - mouse.x_root;
    int yd = e->xbutton.y_root - mouse.y_root;

    XMoveResizeWindow(d, mouse.subwindow,
        wx + (mouse.button == 1 ? xd : 0),
        wy + (mouse.button == 1 ? yd : 0),
        ww + (mouse.button == 3 ? xd : 0),
        wh + (mouse.button == 3 ? yd : 0));
}

void
key_press ( XEvent *e )
{
    KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);

    for (usize i = 0; i < sizeof(keys)/sizeof(*keys); ++i)
        if (keys[i].mod == e->xkey.state && keys[i].keysym == keysym)
            keys[i].function(keys[i].arg);
}

void
button_press ( XEvent *e )
{
    if (!e->xbutton.subwindow) return;

    win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
    XRaiseWindow(d, e->xbutton.subwindow);
    mouse = e->xbutton;
}

void
button_release ( void )
{
    mouse.subwindow = 0;
}

void
win_add ( Window w )
{
    client *c;

    if (!(c = (client *) calloc(1, sizeof(client))))
	{
		EPRINT("sowm: error: unable to allocate memory for new window: ");
		perror("calloc()");
        exit(1);
	}

    c->w = w;

    if (list) {
        list->prev->next = c;
        c->prev          = list->prev;
        list->prev       = c;
        c->next          = list;

    } else {
        list = c;
        list->prev = list->next = list;
    }

    ws_save(ws);
}

void
win_del ( Window w )
{
    client *x = 0;

    for win if (c->w == w) x = c;

    if (!list || !x)  return;
    if (x->prev == x) list = 0;
    if (list == x)    list = x->next;
    if (x->next)      x->next->prev = x->prev;
    if (x->prev)      x->prev->next = x->next;

    free(x);
    ws_save(ws);
}

void
win_kill ( void )
{
    if (cur) XKillClient(d, cur->w);
}

void
win_center ( void )
{
    if (!cur) return;

    win_size(cur->w, &(int){0}, &(int){0}, &ww, &wh);

    XMoveWindow(d, cur->w, (sw - ww) / 2, (sh - wh) / 2);
}

void
win_fs ( void )
{
    if (!cur) return;

    if ((cur->f = cur->f ? 0 : 1)) {
        win_size(cur->w, &cur->wx, &cur->wy, &cur->ww, &cur->wh);
        XMoveResizeWindow(d, cur->w, 0, 0, sw, sh);
    } else
        XMoveResizeWindow(d, cur->w, cur->wx, cur->wy, cur->ww, cur->wh);
}

void
win_to_ws ( const Arg arg )
{
    int tmp = ws;

    if (arg.i == tmp) return;

    ws_sel(arg.i);
    win_add(cur->w);
    ws_save(arg.i);

    ws_sel(tmp);
    win_del(cur->w);
    XUnmapWindow(d, cur->w);
    ws_save(tmp);

    if (list) win_focus(list);
}

void
win_prev ( void )
{
	if (cur->prev->w == screen->root || !cur) return;

	// raise window
	//usize maskvalue[] = { XCB_STACK_MODE_ABOVE };
	//xcb_configure_window(con, cur->prev->w, XCB_CONFIG_WINDOW_STACK_MODE, maskvalue);
	//xcb_flush(con);

    XRaiseWindow(d, cur->prev->w);
    win_focus(cur->prev);
}

void
win_next ( void )
{
	if (cur->next->w == screen->root || !cur) return;

    // raise window	
	//usize value[] = { XCB_STACK_MODE_ABOVE };
	xcb_configure_window(con, cur->next->w, XCB_CONFIG_WINDOW_STACK_MODE, value);
	//xcb_flush(con);

    XRaiseWindow(d, cur->next->w);
    win_focus(cur->next);
}

void
ws_go ( const Arg arg )
{
    int tmp = ws;

    if (arg.i == ws) return;

    ws_save(ws);
    ws_sel(arg.i);

    for win XMapWindow(d, c->w);

    ws_sel(tmp);

    for win XUnmapWindow(d, c->w);

    ws_sel(arg.i);

    if (list) win_focus(list); else cur = 0;
}

void
configure_request ( XEvent *e )
{
    XConfigureRequestEvent *ev = &e->xconfigurerequest;

    XConfigureWindow(d, ev->window, ev->value_mask, &(XWindowChanges) {
        .x          = ev->x,
        .y          = ev->y,
        .width      = ev->width,
        .height     = ev->height,
        .sibling    = ev->above,
        .stack_mode = ev->detail
    });
}

void
map_request ( XEvent *e )
{
    Window w = e->xmaprequest.window;

    XSelectInput(d, w, StructureNotifyMask|EnterWindowMask);
    win_size(w, &wx, &wy, &ww, &wh);
    win_add(w);
    cur = list->prev;

    if (wx + wy == 0) win_center();

    XMapWindow(d, w);
    win_focus(list->prev);
}

void
run ( const Arg arg )
{
    if (fork()) return;
    if (d) close(ConnectionNumber(d));

    setsid();
    execvp((char*)arg.com[0], (char**)arg.com);
}
