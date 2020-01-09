//
// bunnywm
// kiedt's fork of sowm
//
// CHANGELOG
// 	- port to XCB (WIP)
// 	- error message on failed calloc()
// 	- long live kernel style indentation! :P
// 	- apply rounded corner patch
// 	- 2bwm-esque window movement
// 	- misc keybinding changes
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
#include "bunny.h"
#include "config.h"

static int 
xerror(void)
{
	return 0;
}

void
wm_exit(void)
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
	if (!mouse.subwindow || cur->f) return;

	while(XCheckTypedEvent(d, MotionNotify, e));

	int xd = e->xbutton.x_root - mouse.x_root;
	int yd = e->xbutton.y_root - mouse.y_root;

	XMoveResizeWindow(d, mouse.subwindow,
		wx + (mouse.button == 1 ? xd : 0),
		wy + (mouse.button == 1 ? yd : 0),
		ww + (mouse.button == 3 ? xd : 0),
		wh + (mouse.button == 3 ? yd : 0));

	if (mouse.button == 3)
		win_round_corners(mouse.subwindow, CORNER_RADIUS);
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
	if (!e->xbutton.subwindow) return;

	win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
	XRaiseWindow(d, e->xbutton.subwindow);
	mouse = e->xbutton;
}

void
button_release(void)
{
	mouse.subwindow = 0;
}

void
win_add(Window w)
{
	client *c;

	if (!(c = (client *) calloc(1, sizeof(client)))) {
		EPRINT("bunnywm: error: unable to allocate memory for new window: ");
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

	win_round_corners(cur->w, cur->f ? 0 : CORNER_RADIUS);
}

void
win_round_corners(Window w, int rad)
{
	unsigned int ww, wh, dia = 2 * rad;
	win_size(w, &(int){1}, &(int){1}, &ww, &wh);

	if (ww < dia || wh < dia) return;

	Pixmap mask = XCreatePixmap(d, w, ww, wh, 1);
	if (!mask) return;

	XGCValues xgcv;
	GC shape_gc = XCreateGC(d, mask, 0, &xgcv);

	if (!shape_gc) {
		XFreePixmap(d, mask);
		return;
	}

	XSetForeground(d, shape_gc, 0);
	XFillRectangle(d, mask, shape_gc, 0, 0, ww, wh);
	XSetForeground(d, shape_gc, 1);
	XFillArc(d, mask, shape_gc, 0, 0, dia, dia, 0, 23040);
	XFillArc(d, mask, shape_gc, ww-dia-1, 0, dia, dia, 0, 23040);
	XFillArc(d, mask, shape_gc, 0, wh-dia-1, dia, dia, 0, 23040);
	XFillArc(d, mask, shape_gc, ww-dia-1, wh-dia-1, dia, dia, 0, 23040);
	XFillRectangle(d, mask, shape_gc, rad, 0, ww-dia, wh);
	XFillRectangle(d, mask, shape_gc, 0, rad, ww, wh-dia);
	XShapeCombineMask(d, w, ShapeBounding, 0, 0, mask, ShapeSet);
	XFreePixmap(d, mask);
	XFreeGC(d, shape_gc);
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

	win_round_corners(cur->w, CORNER_RADIUS);
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

	win_round_corners(ev->window, CORNER_RADIUS);
}

void
map_request(XEvent *e)
{
	Window w = e->xmaprequest.window;

	XSelectInput(d, w, StructureNotifyMask|EnterWindowMask);
	win_size(w, &wx, &wy, &ww, &wh);
	win_add(w);
	cur = list->prev;

	if ((wx + wy) == 0) win_center();

	xcb_map_window(con, w); //XMapWindow(d, w);
	win_focus(list->prev);

	win_round_corners(w, CORNER_RADIUS);
}

void
run (const Arg arg)
{
	if (fork()) return;
	if (d) close(ConnectionNumber(d));

	setsid();
	execvp((char*) arg.com[0], (char**) arg.com);
}

int
main(void)
{
	XEvent ev;

	if (!(d = XOpenDisplay(0))) {
		EPRINT("bunnywm: error: unable to open X display.\n");
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
