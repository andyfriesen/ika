#!/bin/bash

PYTHONVERSION=`python -c "import sys; print sys.version[:3]"`
PYTHONDIR=-I/usr/include/python$PYTHONVERSION
CFLAGS="-I. -I.. -I/usr/X11R6/include `sdl-config --cflags` $PYTHONDIR"
LDFLAGS="`sdl-config --libs` -L/usr/X11R6/lib -L -lstdc++ -lz -lcorona -laudiere -lutil -lGL -lGLU"

g++ \
{FPSCounter,entity,font,graph,input,main,script,sound,sprite,tileset,timer}.cpp \
   opengl/{Driver,Image}.cpp \
   script/*.cpp \
   ../common/{Canvas,chr,configfile,fileio,fontfile,log,map,misc,rle,vergepal,vsp}.cpp \
   /usr/lib/python$PYTHONVERSION/config/libpython$PYTHONVERSION.a \
   $CFLAGS $LDFLAGS \
   -o ika
