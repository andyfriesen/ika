
all:	fnt2png img2fnt ttf2png

fnt2png:
	cl fnt2png.cpp ../common/canvas.cpp ../common/fontfile.cpp ../common/vergepal.cpp ../common/fileio.cpp -I../common -I../3rdparty/include /EHsc ../3rdparty/lib/corona.lib ../3rdparty/lib/zlib.lib

img2fnt:
	cl fnt2png.cpp ../common/fontfile.cpp ../common/fileio.cpp ../common/vergepal.cpp ../common/canvas.cpp -I../common -I../3rdparty/include /EHsc ../3rdparty/lib/corona.lib ../3rdparty/lib/zlib.lib

ttf2png:
	cl ttf2png.cpp ../common/canvas.cpp -I../common -I../3rdparty/include -I/src/freetype-2.1.2/include /EHsc c:/src/freetype-2.1.2/lib/freetype211ST.lib ../3rdparty/lib/corona.lib
