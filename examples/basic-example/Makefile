
OPTIMIZEFLAG?=-O3
SDL2_CONFIG?=sdl2-config
PKG_CONFIG?=pkg-config

SDLLIBS:=$(shell $(SDL2_CONFIG) --libs)
SDLCFLAGS:=$(shell $(SDL2_CONFIG) --cflags)

GLEWLIBS:=$(shell $(PKG_CONFIG) --libs-only-l glew)
GLEWCFLAGS:=$(shell $(PKG_CONFIG) --cflags glew)

CFLAGS=${GLEWCFLAGS} -I../../renderer -pthread -fsanitize=bounds ${SDLCFLAGS} ${OPTIMIZEFLAG}

LIBS=-lsnis-renderer ${SDLLIBS} ${GLEWLIBS} -lm -lpng

all:	main

main:	main.c ../../renderer/libsnis-renderer.so
	$(CC) ${CFLAGS} -o main -I../../renderer main.c -L../../renderer ${LIBS}

../../renderer/libsnis-renderer.so:
	(cd ../../renderer && make)

clean:
	rm -f main

