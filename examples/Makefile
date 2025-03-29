CC		= cc
CFLAGS	= -Wall -Wextra -Werror -std=c99
LFLAGS	= -lxgl -lX11 -lGL -lGLX
LDFLAGS	= -L ..

all : release

.PHONY : release debug

release : CFLAGS += -s -O3
release : 00-example-window.out 01-example-clear-window.out 02-example-multiple-windows.out 03-example-input.out

debug : CFLAGS += -ggdb -g3 -O0
debug : 00-example-window.out 01-example-clear-window.out 02-example-multiple-windows.out 03-example-input.out

00-example-window.out :
	$(CC) $(CFLAGS) 00-example-window.c -o $@ $(LFLAGS) $(LDFLAGS)

01-example-clear-window.out :
	$(CC) $(CFLAGS) 01-example-clear-window.c -o $@ $(LFLAGS) $(LDFLAGS)

02-example-multiple-windows.out :
	$(CC) $(CFLAGS) 02-example-multiple-windows.c -o $@ $(LFLAGS) $(LDFLAGS)

03-example-input.out :
	$(CC) $(CFLAGS) 03-example-input.c -o $@ $(LFLAGS) $(LDFLAGS)

.PHONY : clean

clean:
	rm -f ./*.out
