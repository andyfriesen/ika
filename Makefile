CC = g++
PYTHONINC = -I`python -c "import sys; print sys.path[1].replace('lib','include')"`
CPPFLAGS = -I. -I.. -I/usr/X11R6/include `sdl-config --cflags` $(PYTHONINC)
LDFLAGS = `python-config` -lGLU -lGL -lSDL -L/usr/X11R6/lib \
    -lcorona -laudiere -lstdc++ -lutil -lz
SUBDIRS = common engine \
	 # ikaMap <-- doesn't run yet

all: $(SUBDIRS)

common: libcommon.la
libcommon.la:
	(cd common; make CC=$(CC) LDFLAGS="$(LDFLAGS)")

engine: engine/ika
engine/ika:
	(cd engine; make CC=$(CC) CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LDFLAGS)")

ikaMap: ikaMap/ikamap
ikaMap/ikamap:
	(cd ikaMap; make CC=$(CC) CPPFLAGS="$(CPPFLAGS) -g -I../common `wx-config --cppflags`" LDFLAGS="$(LDFLAGS) `wx-config --libs` -lwx_gtk2_gl-2.4 -lwx_gtk2_xrc-2.4")

subdirs:
	rm engine/ika libcommon.la
	make all

clean:
	(for d in $(SUBDIRS); do cd $$d; make clean; cd ..; done)
