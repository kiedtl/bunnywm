# bunnywm

An small, XCB-based window manager forked from sowm with extra features.

## Why?
Minimalism is good -- but it should not be so extreme that is prevents you
from getting things done.

- partial EWMH support
	- \_NET\_WM\_PID
	- \_NET\_WM\_NAME
	- \_NET\_NUMBER\_OF\_DESKTOPS
	- \_NET\_CURRENT\_DESKTOP
- (primitive) 2bwm-esque window movement
- rounded corners by default
	- easily disabled by editing `CORNER_RADIUS`.
- XCB instead of libX11 (*WIP**)
- long live kernel-style indentation! :P

At the same time, bunnywm strives to keep the spirit of sowm:
- 450 loc (*excluding comments/blanks*)
- *22k* compiled size (*with optimizations*)

<br>

## Default Keybindings

**Window Management**

| combo                              | action                 |
| ---------------------------------- | -----------------------|
| `Mouse`                            | focus under cursor     |
| `MOD4` + `Left Mouse`              | move window            |
| `MOD4` + `Right Mouse`             | resize window          |
| `MOD4` + `f`                       | maximize toggle        |
| `MOD4` + `c`                       | center window          |
| `MOD4` + `Shift` + `q`             | kill window            |
| `MOD4` + `1-9`                     | desktop swap           |
| `MOD4` + `Shift` + `1-9`           | send window to desktop |
| `MOD4` + `j`                       | focus next window      |
| `MOD4` + `k`                       | focus previous window  |
| `MOD4` + `y`/`u`/`i`/`o`           | resize window          |
| `MOD4` + `Shift` + `h`/`j`/`k`/`l` | move window    |

**Programs**

> NOTE: volume/brightness programs must be define by the user.

| combo                    | action           | program        |
| ------------------------ | ---------------- | -------------- |
| `MOD4` + `Return`        | terminal         | `st`           |
| `MOD4` + `p`             | dmenu            | `dmenu_run`    |
| `MOD4` + `Shift` + `s`   | screen lock      | `slock`        |
| `MOD4` + `Shift` + `p`   | scrot            | `scr`          |
| `XF86_AudioLowerVolume`  | volume down      | ``             |
| `XF86_AudioRaiseVolume`  | volume up        | ``             |
| `XF86_AudioMute`         | volume toggle    | ``             |
| `XF86_MonBrightnessUp`   | brightness up    | ``             |
| `XF86_MonBrightnessDown` | brightness down  | ``             |


## Dependencies

- `libxcb`
- `libXext`
- `libX11-xcb` (will be removed soon)
- `libX11` (will be removed soon)


## Installation

1) Modify `config.h` to suit your needs.
2) Run `make` to build `bunny`.
3) Copy it to your path or run `make install`.
    - `DESTDIR` and `PREFIX` are supported.


## Thanks

- sowm
- 2bwm
- catwm
- dwm
- frankenwm
- tinywm
