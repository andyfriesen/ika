# cygwin specific.
#CC=g++
#CPPFLAGS=-I../3rdparty/include -I.. -I/usr/include/python2.3

OBJECTS = \
    FPSCounter.o \
    entity.o \
    font.o \
    input.o \
    main.o \
    script.o \
    scriptobject.o \
    sound.o \
    sprite.o \
    tileset.o \
    timer.o \
    opengl/Driver.o \
    opengl/Image.o \
    script/CanvasObject.o \
    script/ControlObject.o \
    script/EntityObject.o \
    script/ErrorObject.o \
    script/FontObject.o \
    script/ImageObject.o \
    script/InputObject.o \
    script/MapObject.o \
    script/ModuleFuncs.o \
    script/SoundObject.o \
    script/VideoObject.o

all: ika
ika: $(OBJECTS)
	libtool --mode=link $(CC) -o $@ $(CPPFLAGS) ../libcommon.la $(OBJECTS) $(LDFLAGS)

clean:
	rm -f ika *.o opengl/*.o script/*.o
