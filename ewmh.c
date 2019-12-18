// "junk" is easier to type than "EWMH" :P

#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <string.h>
#include "types.h"
#include "ewmh.h"

void
junk_init(xcb_connection_t *con)
{
		if (!(junk = calloc(1, sizeof(xcb_ewmh_connection_t)))) {
				fprintf(stderr, "sowm: error: unable to initialize EWMH: ");
				perror("calloc()");
		}

		xcb_intern_atom_cookie_t *cookie = xcb_ewmh_init_atoms(con, junk);
		if (!xcb_ewmh_init_atoms_replies(junk, cookie, (void*) 0)) {
				fprintf(stderr, "sowm: error: unable to initialize EWMH: ");
				perror("xcb_ewmh_init_atoms_replies()");
		}
}

void
junk_set_wmname(char *name, xcb_screen_t *screen)
{
		xcb_ewmh_set_wm_name(junk, screen->root, strlen(name), name);
}

void
junk_set_currentws(usize w)
{
		 xcb_ewmh_set_current_desktop(junk, 0, w);
}

void
junk_set_maxws(usize max)
{
		xcb_ewmh_set_number_of_desktops(junk, 0, max);
}

void
junk_set_activew(Window w, xcb_connection_t *con, xcb_screen_t *screen)
{
		xcb_change_property(con, XCB_PROP_MODE_REPLACE, screen->root,
						junk->_NET_ACTIVE_WINDOW, XCB_ATOM_WINDOW, 32, 1, 
						(const void*) w);
}
