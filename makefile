NAME		 = sowm

WARNINGS	 = -Wall -Wextra -pedantic -Wmissing-prototypes \
			   -Wold-style-definition -Werror

CC 			?= gcc
CFLAGS		 = -std=c99 -O3 $(WARNINGS)
LDFLAGS	 	 = -lX11
PREFIX		?= /usr/local
BINDIR		?= $(PREFIX)/bin

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
