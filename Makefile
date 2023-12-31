CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
LIBS = -lm -lMagickWand



all: inspector convert

inspector: inspector.c bapl.h
	$(CC) $(CFLAGS) inspector.c -o inspector $(LIBS)

convert: convert.c bapl.h
	export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
	$(CC) $(CFLAGS) convert.c -o convert `pkg-config --cflags --libs MagickWand` 

viewer: viewer.c bapl.h
	export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
	clang -o viewer viewer.c -lSDL2 $(pkg-config --cflags --libs sdl2)

clean:
	rm -f inspector convert

