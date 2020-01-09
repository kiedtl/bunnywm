//
// kiedtl's sowm
//

// TODO: convert all indent to tabs

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

static int 
xerror(void)
{
	return 0;
}

void
sowm_exit(void)
{
	exit(EXIT_SUCCESS);
}

void
win_focus(client *c)
{
	if (c == NULL) return;

	cur = c;
	xcb_set_input_focus(con, XCB_INPUT_FOCUS_PARENT, cur->w, XCB_CURRENT_TIME);
	xcb_flush(con);

	//XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
}

void
notify_destroy(XEvent *e)
{
	win_del(e->xdestroywindow.window);

	if (list) win_focus(list->prev);
}

void
notify_enter(XEvent *e)
{
	while(XCheckTypedEvent(d, EnterNotify, e));

	for win if (c->w == e->xcrossing.window) win_focus(c);
}

void
notify_motion(XEvent *e)
{
	if (!mouse.subwindow || cur->f) {
		draw_outline(mouse.x_root, mouse.y_root, e->xbutton.x_root, e->xbutton.y_root);
		return;
	}

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
key_press(XEvent *e)
{
	KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);

	for (usize i = 0; i < sizeof(keys)/sizeof(*keys); ++i)
	    if (keys[i].mod == e->xkey.state && keys[i].keysym == keysym)
	        keys[i].function(keys[i].arg);
}

void
button_press(XEvent *e)
{
	mouse = e->xbutton;

	if (!e->xbutton.subwindow) return;
	win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
	XRaiseWindow(d, e->xbutton.subwindow);
}

void
button_release(XEvent *e)
{
    XClearWindow(d, RootWindow(d, DefaultScreen(d)));

    if (WaitingWindow) {
        XSelectInput(d, WaitingWindow, StructureNotifyMask|EnterWindowMask);
        win_size(WaitingWindow, &wx, &wy, &ww, &wh);
        win_add(WaitingWindow);
        cur = list->prev;

        XMoveResizeWindow(d, cur->w,
            e->xbutton.x_root > mouse.x_root ? mouse.x_root : e->xbutton.x_root,
            e->xbutton.y_root > mouse.y_root ? mouse.y_root : e->xbutton.y_root,
            ABS(mouse.x_root - e->xbutton.x_root),
            ABS(mouse.y_root - e->xbutton.y_root));

        XMapWindow(d, WaitingWindow);
        win_focus(list->prev);

        WaitingWindow = 0;

    } else if (!mouse.subwindow && cur) {

        XMoveResizeWindow(d, cur->w,
            e->xbutton.x_root > mouse.x_root ? mouse.x_root : e->xbutton.x_root,
            e->xbutton.y_root > mouse.y_root ? mouse.y_root : e->xbutton.y_root,
            ABS(mouse.x_root - e->xbutton.x_root),
            ABS(mouse.y_root - e->xbutton.y_root));

        mouse.subwindow = 0;
    }
}

void
win_add(Window w)
{
	client *c;

	if (!(c = (client *) calloc(1, sizeof(client)))) {
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
win_del(Window w)
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
win_kill(void)
{
	if (cur) XKillClient(d, cur->w);
}

void
win_center(void)
{
	if (!cur) return;

	win_size(cur->w, &(int){0}, &(int){0}, &ww, &wh);

	XMoveWindow(d, cur->w, (sw - ww) / 2, (sh - wh) / 2);
}

void
win_fs(void)
{
	if (!cur) return;

	if ((cur->f = cur->f ? 0 : 1)) {
	    win_size(cur->w, &cur->wx, &cur->wy, &cur->ww, &cur->wh);
	    XMoveResizeWindow(d, cur->w, 0, 0, sw, sh);
	} else {
	    XMoveResizeWindow(d, cur->w, cur->wx, cur->wy, cur->ww, cur->wh);
	}
}

void
win_to_ws(const Arg arg)
{
	if (MAX_WS < arg.i)
			return;

	int tmp = ws;

	if (arg.i == tmp) return;

	ws_sel(arg.i);
	win_add(cur->w);
	ws_save(arg.i);

	ws_sel(tmp);
	win_del(cur->w);
	xcb_unmap_window(con, cur->w); //XUnmapWindow(d, cur->w);
	ws_save(tmp);

	if (list) win_focus(list);
}

void
win_prev(void)
{
	if (!cur) return;

	// raise window
	usize maskvalue[] = { XCB_STACK_MODE_ABOVE };
	xcb_configure_window(con, cur->prev->w, XCB_CONFIG_WINDOW_STACK_MODE, maskvalue);
	xcb_flush(con);

	//XRaiseWindow(d, cur->prev->w);
	win_focus(cur->prev);
}

void
win_next(void)
{
	if (!cur) return;

	// raise window	
	usize value[] = { XCB_STACK_MODE_ABOVE };
	xcb_configure_window(con, cur->next->w, XCB_CONFIG_WINDOW_STACK_MODE, value);
	xcb_flush(con);

	//XRaiseWindow(d, cur->next->w);
	win_focus(cur->next);
}

void
win_modify(const Arg arg)
{
		usize step = 20;

		if (!cur || cur->f) return;

		// raise window
		usize values[] = { XCB_STACK_MODE_ABOVE };
		xcb_configure_window(con, cur->w, XCB_CONFIG_WINDOW_STACK_MODE, values);
		xcb_flush(con);

		// update wh ww wy wx
		win_size(cur->w, &cur->wx, &cur->wy, &cur->ww, &cur->wh);

		// move/resize up or down, left or right
		switch (arg.i)
		{
		case RESIZE_LEFT:
				cur->ww = cur->ww - step;
				values[0] = cur->ww;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_WIDTH, values);
				break;
		case RESIZE_RIGHT:
				cur->ww = cur->ww + step;
				values[0] = cur->ww;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_WIDTH, values);
				break;
		case RESIZE_UP:
				cur->wh = cur->wh - step;
				values[0] = cur->wh;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_HEIGHT, values);
				break;
		case RESIZE_DOWN:
				cur->wh = cur->wh + step;
				values[0] = cur->wh;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_HEIGHT, values);
				break;
		case MOVE_UP:
				cur->wy = cur->wy - step;
				values[0] = cur->wy;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_Y, values);
				break;
		case MOVE_DOWN:
				cur->wy = cur->wy + step;
				values[0] = cur->wy;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_Y, values);
				break;
		case MOVE_RIGHT:
				cur->wx = cur->wx + step;
				values[0] = cur->wx;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_X, values);
				break;
		case MOVE_LEFT:
				cur->wx = cur->wx - step;
				values[0] = cur->wx;
				xcb_configure_window(con, cur->w,
								XCB_CONFIG_WINDOW_X, values);
				break;
		}

		xcb_flush(con);
}

void
ws_go ( const Arg arg )
{
	if (MAX_WS < arg.i)
			return; // get the hell outta here

	int tmp = ws;

	if (arg.i == ws) return;

	ws_save(ws);
	ws_sel(arg.i);

	for win xcb_map_window(con, c->w); //XMapWindow(d, c->w);

	ws_sel(tmp);

	for win xcb_unmap_window(con, c->w); //XUnmapWindow(d, c->w);

	ws_sel(arg.i);

	if (list) win_focus(list);
	else cur = 0;
}

void
configure_request(XEvent *e)
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
map_request(XEvent *e)
{
	WaitingWindow = e->xmaprequest.window;
}

void
run (const Arg arg)
{
	if (fork()) return;
	if (d) close(ConnectionNumber(d));

	setsid();
	execvp((char*) arg.com[0], (char**) arg.com);
}

void
draw_outline(int x1, int y1, int x2, int y2)
{
	XClearWindow(d, RootWindow(d, DefaultScreen(d)));

	GC gc = XCreateGC(d, RootWindow(d, DefaultScreen(d)), 0, NULL);
	if (!gc) return;

	XSetForeground(d, gc, WhitePixel(d, DefaultScreen(d)));
	XDrawLine(d, RootWindow(d, DefaultScreen(d)), gc, x1, y1, x1, y2);
	XDrawLine(d, RootWindow(d, DefaultScreen(d)), gc, x1, y1, x2, y1);
	XDrawLine(d, RootWindow(d, DefaultScreen(d)), gc, x1, y2, x2, y2);
	XDrawLine(d, RootWindow(d, DefaultScreen(d)), gc, x2, y1, x2, y2);
	XFreeGC(d, gc);
	XFlush(d);
}

int
main(void)
{
	XEvent ev;

	if (!(d = XOpenDisplay(0))) {
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
