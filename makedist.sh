#!/usr/bin/bash

if [ -z $1 ]
    then
        echo "Gimme a version number, dork."
        exit
    else
        version=$1
    fi

if [ $MACHTYPE==i686-pc-cygwin ]
    then
        echo Assembling core zip...
        mkdir tempdist
        cp engine/Release/ika.exe tempdist
        cp iked/Release/iked.exe tempdist
        cp ikamap/Release/ikamap.exe tempdist
        cp tools/*.exe tempdist
        cd tempdist
        upx *.exe
        zip ../ika-core-$version.zip *
        cd ..
        #rm -rf tempdist
        echo Done.

        echo Assembling the main dist zip...
        #mkdir tempdist
        mkdir tempdist/xi
        cp -R dist/* tempdist
        cp xi/* tempdist/xi
        cp 3rdparty/dlls/{audiere,corona,msvc*71,python23,sdl,zlib}.dll tempdist
        #cp engine/Release/ika.exe tempdist
        #cp iked/Release/iked.exe tempdist
        #cp ikamap/Release/ikamap.exe tempdist
        #cp tools/*.exe tempdist
        cd tempdist
        zip -r ../ika-win-$version.zip *
        cd ..
        echo Done.

        echo Creating NSIS installer.
        cd tempdist
        cp ../ika.nis .
        /cygdrive/d/Program\ Files/NSIS/makensis.exe ika.nis && mv ika-install-$version.exe ..
        rm ika.nis
        cd ..
        echo Done.
        rm -rf tempdist

        echo Assembling source archive...
        mkdir ika
        cd ika
        cvs -z3 -d:pserver:anonymous@cvs.ika.sourceforge.net:/cvsroot/ika export -D today common engine iked ikaMap xi
        cd ..
        tar cjf ika-src-$version.tar.bz2 ika
        #rm -rf ika
        echo Done.
    else
        echo "This don't work on nonwindows yet."
        echo "TODO:  Correct this."
    fi
