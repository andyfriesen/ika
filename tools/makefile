CC=cl
CCPP=cl
CPPFLAGS=/I.. /I../3rdparty/include /I../3rdparty/include/freetype /EHsc
LINKFLAGS=../3rdparty/lib/corona.lib ../3rdparty/lib/zlib.lib ../3rdparty/lib/freetype218ST.lib

all: fnt2png img2fnt ttf2png

fnt2png:
	cl fnt2png.cpp ../common/canvas.cpp ../common/fontfile.cpp ../common/vergepal.cpp ../common/fileio.cpp $(CPPFLAGS) $(LINKFLAGS)

img2fnt:
	cl img2fnt.cpp ../common/fontfile.cpp ../common/fileio.cpp ../common/vergepal.cpp ../common/canvas.cpp $(CPPFLAGS) $(LINKFLAGS)

ttf2png:
	cl ttf2png.cpp ../common/canvas.cpp $(CPPFLAGS) $(LINKFLAGS)
