# Microsoft Developer Studio Project File - Name="ika" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ika - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ika.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ika.mak" CFG="ika - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ika - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ika - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /I "..\3rdparty" /I "..\common" /I "..\3rdparty\include" /I "..\3rdparty\lib" /I "..\3rdparty\python" /I "G:\ika\src\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSVC" /D "INLINE_ASM" /D "MSVC6" /D "OPENGL_VIDEO" /FD /c
# SUBTRACT CPP /X /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib G:\Ika\src\3rdparty\zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib corona.lib sdl.lib sdlmain.lib audiere.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386 /out:"G:\Ika\Ika.exe" /libpath:"..\common\\" /libpath:"$(PYTHON)\libs" /libpath:"..\3rdparty"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "..\3rdparty" /I "..\common" /I "..\3rdparty\include" /I "..\3rdparty\lib" /I "..\3rdparty\python" /I "G:\ika\src\engine" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSVC" /D "INLINE_ASM" /D "MSVC6" /D "SOFT32_VIDEO" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib G:\Ika\src\3rdparty\zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib corona.lib sdl.lib sdlmain.lib audiere.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"G:\ika\ikadebug.exe" /pdbtype:sept /libpath:"..\common\\" /libpath:"$(PYTHON)\libs" /libpath:"..\3rdparty"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "ika - Win32 Release"
# Name "ika - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\benchmark.cpp
# End Source File
# Begin Source File

SOURCE=.\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\FPSCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\tileset.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\benchmark.h
# End Source File
# Begin Source File

SOURCE=.\video\Driver.h
# End Source File
# Begin Source File

SOURCE=.\entity.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\FPSCounter.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=.\hooklist.h
# End Source File
# Begin Source File

SOURCE=.\video\Image.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\sprite.h
# End Source File
# Begin Source File

SOURCE=.\tileset.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\engine.rc
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\Canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Canvas.h
# End Source File
# Begin Source File

SOURCE=..\common\CanvasBlitter.h
# End Source File
# Begin Source File

SOURCE=..\common\chr.cpp
# End Source File
# Begin Source File

SOURCE=..\common\chr.h
# End Source File
# Begin Source File

SOURCE=..\common\configfile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\configfile.h
# End Source File
# Begin Source File

SOURCE=..\common\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\common\fileio.h
# End Source File
# Begin Source File

SOURCE=..\common\fontfile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\fontfile.h
# End Source File
# Begin Source File

SOURCE=..\common\log.cpp
# End Source File
# Begin Source File

SOURCE=..\common\log.h
# End Source File
# Begin Source File

SOURCE=..\common\map.cpp
# End Source File
# Begin Source File

SOURCE=..\common\map.h
# End Source File
# Begin Source File

SOURCE=..\common\mem.cpp
# End Source File
# Begin Source File

SOURCE=..\common\mem.h
# End Source File
# Begin Source File

SOURCE=..\common\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\common\misc.h
# End Source File
# Begin Source File

SOURCE=..\common\rle.cpp
# End Source File
# Begin Source File

SOURCE=..\common\rle.h
# End Source File
# Begin Source File

SOURCE=..\common\types.h
# End Source File
# Begin Source File

SOURCE=..\common\vergepal.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vergepal.h
# End Source File
# Begin Source File

SOURCE=..\common\vsp.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vsp.h
# End Source File
# End Group
# Begin Group "Script"

# PROP Default_Filter "py*.*"
# Begin Source File

SOURCE=.\script\CanvasObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\ControlObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\EntityObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\ErrorObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\FontObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\ImageObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\InputObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\MapObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\ModuleFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\script\ObjectDefs.h
# End Source File
# Begin Source File

SOURCE=.\script\SoundObject.cpp
# End Source File
# Begin Source File

SOURCE=.\script\VideoObject.cpp
# End Source File
# End Group
# Begin Group "GL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\opengl\Driver.cpp

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\opengl"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\opengl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opengl\Driver.h

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\opengl"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\opengl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opengl\Image.cpp

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\opengl"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\opengl"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\opengl\Image.h

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\opengl"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\opengl"

!ENDIF 

# End Source File
# End Group
# Begin Group "Soft32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\soft32\Driver.cpp

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\soft32"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\soft32"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soft32\Driver.h

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\soft32"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\soft32"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soft32\Image.cpp

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\soft32"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\soft32"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soft32\Image.h

!IF  "$(CFG)" == "ika - Win32 Release"

# PROP Intermediate_Dir "Release\soft32"

!ELSEIF  "$(CFG)" == "ika - Win32 Debug"

# PROP Intermediate_Dir "Debug\soft32"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\soft32\Misc.h
# End Source File
# End Group
# End Target
# End Project
