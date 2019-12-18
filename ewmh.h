#ifndef EWMH_H
#define EWMH_H

#include <X11/Xlib.h>
#include <xcb/xcb_ewmh.h>
#include "types.h"

xcb_ewmh_connection_t *junk;

void junk_init(xcb_connection_t *con);
void junk_set_wmname(char *name, xcb_screen_t *screen);
void junk_set_currentws(usize w);
void junk_set_maxws(usize max);
void junk_set_activew(Window w, 
				xcb_connection_t *con, 
				xcb_screen_t *screen);

#endif
