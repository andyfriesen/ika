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
# ADD CPP /nologo /W3 /GX /O2 /I "..\common" /I "$(PYTHON)\include" /I "..\3rdparty" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dinput.lib dxguid.lib winmm.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\common\\" /libpath:"$(PYTHON)\libs" /libpath:"..\3rdparty"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\python" /I "$(PYTHON)\include" /I "..\3rdparty" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dinput.lib dxguid.lib winmm.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\common\\" /libpath:"$(PYTHON)\libs" /libpath:"..\3rdparty"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ika - Win32 Release"
# Name "ika - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
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
# Begin Source File

SOURCE=.\win32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\controller.h
# End Source File
# Begin Source File

SOURCE=.\entity.h
# End Source File
# Begin Source File

SOURCE=.\font.h
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
# Begin Source File

SOURCE=.\win32.h
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

SOURCE=..\common\chr.cpp
# End Source File
# Begin Source File

SOURCE=..\common\chr.h
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

SOURCE=..\common\graph.cpp
# End Source File
# Begin Source File

SOURCE=..\common\graph.h
# End Source File
# Begin Source File

SOURCE=..\common\importpng.cpp
# End Source File
# Begin Source File

SOURCE=..\common\importpng.h
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

SOURCE=..\common\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\common\misc.h
# End Source File
# Begin Source File

SOURCE=..\common\pixel_matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\common\pixel_matrix.h
# End Source File
# Begin Source File

SOURCE=..\common\rle.cpp
# End Source File
# Begin Source File

SOURCE=..\common\rle.h
# End Source File
# Begin Source File

SOURCE=..\common\strk.cpp
# End Source File
# Begin Source File

SOURCE=..\common\strk.h
# End Source File
# Begin Source File

SOURCE=..\common\types.h
# End Source File
# Begin Source File

SOURCE=..\common\v_config.cpp
# End Source File
# Begin Source File

SOURCE=..\common\v_config.h
# End Source File
# Begin Source File

SOURCE=..\common\vergepal.cpp
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

SOURCE=.\py_entity.h
# End Source File
# Begin Source File

SOURCE=.\py_error.h
# End Source File
# Begin Source File

SOURCE=.\py_font.h
# End Source File
# Begin Source File

SOURCE=.\py_image.h
# End Source File
# Begin Source File

SOURCE=.\py_input.h
# End Source File
# Begin Source File

SOURCE=.\py_map.h
# End Source File
# Begin Source File

SOURCE=.\py_music.h
# End Source File
# Begin Source File

SOURCE=.\py_sound.h
# End Source File
# Begin Source File

SOURCE=.\python_stdlib.h
# End Source File
# End Group
# End Target
# End Project
