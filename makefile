NAME		 = sowm
PREFIX		?= /usr/local
BINDIR		?= $(PREFIX)/bin

WARNINGS	 = -Wall -Wextra -pedantic -Wmissing-prototypes \
			   -Wold-style-definition -Werror -Wno-incompatible-pointer-types

CC 			?= gcc
INC			 = -Isub/ccommon/ -I/usr/include/X11/ -I/usr/include/ 
CFLAGS		 = -O3 -std=c99 $(WARNINGS) $(INC)
LDFLAGS	 	 = -lX11 -lxcb -lX11-xcb

SRC			 = $(NAME).c
OBJ			 = $(SRC:.c=.o)

all: config.h $(NAME)

config.h:
	cp config.def.h config.h

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -O3 -o $@ $<

install: all
	install -Dm 755 sowm $(DESTDIR)$(BINDIR)/sowm

clean:
	rm -f $(NAME) $(OBJ)

test:
	for patch in patches/*.patch; do patch --dry-run -p1 < "$$patch"; done
