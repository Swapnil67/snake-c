PKGS=sdl2
CFLAGS=-Wall -Wextra -Wconversion -pedantic -g `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)`

snake: snake.c
	$(CC) $(CFLAGS) snake.c -o snake $(LIBS)