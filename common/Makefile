# cygwin specific
#CC=g++
#INCLUDES=-I../3rdparty/include

OBJECTS = \
    aries.lo \
    base64.lo \
    Canvas.lo \
    chr.lo \
    compression.lo \
    configfile.lo \
    fileio.lo \
    fontfile.lo \
    log.lo \
    map.lo \
    misc.lo \
    rle.lo \
    vergepal.lo \
    vsp.lo

.SUFFIXES: .lo

all: ../libcommon.la

.cpp.lo:
	libtool $(CC) $(INCLUDES) -c $<

../libcommon.la: $(OBJECTS)
	libtool $(CC) $(INCLUDES) -o $@ $(OBJECTS)

clean:
	rm -f ../libcommon.la *.lo
