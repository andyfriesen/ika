#!/usr/bin/bash

if [ -z $1 ]
    then
        echo "Gimme a version number, dork."
        exit
    else
        version=$1
    fi

makedist_dir=makedist_temp
demo_dir=ika-$version
core_dir=core
src_dir=src
dll_dir=dll

original_path=`PWD`
makedist_path=$original_path/$makedist_dir
demo_path=$makedist_path/$demo_dir
core_path=$makedist_path/$core_dir
src_path=$makedist_path/$src_dir
dll_path=$makedist_path/$dll_dir

if [ $MACHTYPE==i686-pc-cygwin ]
    then

        echo Creating temporary directories.

            mkdir $makedist_path
            mkdir $core_path
            mkdir $dll_path
            mkdir $src_path
            mkdir $demo_path

        echo Assembling core zip...

            cp {engine,iked,ikamap}/Release/*.exe $core_path
            cp tools/*.exe $core_path
            pushd $core_path
                upx *.exe
            popd
            zip ika-core-$version.zip $core_path/*.exe

        echo Done.

        echo Assembling DLL zip

            cp 3rdparty/dlls/audiere.dll $dll_path
            cp 3rdparty/dlls/corona.dll $dll_path
            cp 3rdparty/dlls/msvcp71.dll $dll_path
            cp 3rdparty/dlls/msvcr71.dll $dll_path
            cp 3rdparty/dlls/python23.dll $dll_path
            cp 3rdparty/dlls/zlib.dll $dll_path
            cp 3rdparty/dlls/sdl.dll $dll_path

            zip ika-dlls-$version.zip $dll_path/*.dll

        echo Done

        echo Assembling the main dist zip...
            cp -R dist/* $demo_path

            cp $core_path/*.exe $demo_path
            cp $dll_path/*.dll $demo_path

            pushd $demo_path/..
            zip -r $original_path/ika-win-$version.zip $demo_dir/*
            popd
        echo Done.

        echo Creating NSIS installer.
            pushd $demo_path

            #echo `sed "s/@@VERSION@@/$version/g" < $original_path/ika.nis > $demo_path/ika.nis`
            sed "s/@@VERSION@@/$version/g" < $original_path/ika.nis > $demo_path/ika.nis
            #(sed "s/@@VERSION@@/$version/g" < $original_path/ika.nis > $demo_path/ika.nis) || echo "FUCK" && exit

            /cygdrive/d/Program\ Files/NSIS/makensis.exe ika.nis && \
                mv ika-install-$version.exe $original_path

            popd
        echo Done.

        echo Assembling source archive...
            pushd $src_path
                cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/ika export -D today common engine iked ikaMap xi
                # hack, since cvs won't pull it in on its own:
                cp $original_path/SConstruct $src_path
            popd

            pushd $src_path/..
                tar cjf $original_path/ika-src-$version.tar.bz2 $src_dir
            popd
        echo Done.
    else
        echo "This don't work on nonwindows yet."
        echo "TODO:  Correct this."
    fi
