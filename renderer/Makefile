CC?=gcc
OPTIMIZEFLAG?=-O3
PKG_CONFIG?=pkg-config

MYCFLAGS=-DDESTDIR=${DESTDIR} -DPREFIX=${PREFIX} ${DEBUGFLAG} ${PROFILEFLAG} \
	${OPTIMIZEFLAG} ${ASANFLAG} ${UBSANFLAG}\
	--pedantic -Wall -Wextra -Wformat -Wformat=2 -Werror=format-security ${STOP_ON_WARN} -pthread -std=gnu99 ${RDYNAMIC} \
	-Wvla \
	-DUSE_SNIS_XWINDOWS_HACKS=${USE_SNIS_XWINDOWS_HACKS} -fno-common \
	-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fsanitize=bounds \
	-Warray-bounds \
	-fstack-clash-protection -fstack-protector-strong -Wimplicit-fallthrough \
	-Wl,-z,nodlopen -Wl,-z,noexecstack \
	-Wl,-z,relro -Wl,-z,now \
	-Wl,--as-needed -Wl,--no-copy-dt-needed-entries \
	${COMPSPECCFLAGS} -Wstrict-prototypes -fexceptions -Wshadow

OBJS=graph_dev.o quat.o matrix.o snis_graph.o mathutils.o vec4.o entity.o workqueue.o pthread_util.o shader.o stacktrace.o string-utils.o \
	opengl_cap.o png_utils.o liang-barsky.o bline.o ui_colors.o mtwist.o snis_alloc.o mesh.o mikktspace/mikktspace.o \
	open-simplex-noise.o material.o

PNGLIBS:=$(shell $(PKG_CONFIG) --libs libpng)
PNGCFLAGS:=$(shell $(PKG_CONFIG) --cflags libpng)
GLEWLIBS:=$(shell $(PKG_CONFIG) --libs-only-l glew)
GLEWCFLAGS:=$(shell $(PKG_CONFIG) --cflags glew)

CFLAGS=${MYCFLAGS} ${PNGCFLAGS} ${GLEWCFLAGS}

LIBS=${GLEWLIBS} ${PNGLIBS} -lm

all:	libsnis-renderer.so

%.o:	%.c %.h
	${CC} ${CFLAGS} -fpic -c -o $@ $<

graph_dev.o:	graph_dev.h graph_dev_opengl.c
	${CC} ${CFLAGS} -fpic -c -o graph_dev.o graph_dev_opengl.c

libsnis-renderer.so:	${OBJS}
	gcc -shared -o libsnis-renderer.so ${OBJS}


mikktspace/mikktspace.o:	mikktspace/mikktspace.c mikktspace/mikktspace.h mikktspace/Makefile
	cd mikktspace && make

clean:
	rm -f *.o libsnis-renderer.so
	cd mikktspace && make clean

