#!/bin/bash

g++ \
   {FPSCounter,entity,font,graph,input,main,script,sound,sprite,tileset,timer}.cpp \
   opengl/{Driver,Image}.cpp \
   script/*.cpp \
   ../common/{Canvas,chr,configfile,fileio,fontfile,log,map,misc,rle,vergepal,vsp}.cpp \
   -I.  -I..  \
   `sdl-config --cflags --libs` \
   -I/usr/include/python`python -c "import sys; print sys.version[:3]"` \
   -lz -lcorona -laudiere -lutil -lGL -lGLU \
   -o ika
