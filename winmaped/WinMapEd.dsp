# Microsoft Developer Studio Project File - Name="winmaped" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=winmaped - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinMapEd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinMapEd.mak" CFG="winmaped - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winmaped - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "winmaped - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "winmaped - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\common" /I "$(PYTHON)\include" /I "..\3rdparty" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msimg32.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\common" /libpath:"$(PYTHON)\lib" /libpath:"..\3rdparty"

!ELSEIF  "$(CFG)" == "winmaped - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\common" /I "$(PYTHON)\include" /I "..\3rdparty" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\common" /libpath:"$(PYTHON)\lib" /libpath:"..\3rdparty"

!ENDIF 

# Begin Target

# Name "winmaped - Win32 Release"
# Name "winmaped - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\chred.cpp
# End Source File
# Begin Source File

SOURCE=.\coolstuff.cpp
# End Source File
# Begin Source File

SOURCE=.\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\dib.cpp
# End Source File
# Begin Source File

SOURCE=.\entityed.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\importchrdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\importvspdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\layerdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\mapdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\mapview.cpp
# End Source File
# Begin Source File

SOURCE=.\miscdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\pix_edit.cpp
# End Source File
# Begin Source File

SOURCE=.\tileed.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesel.cpp
# End Source File
# Begin Source File

SOURCE=.\winmaped.cpp
# End Source File
# Begin Source File

SOURCE=.\zoneed.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\chred.h
# End Source File
# Begin Source File

SOURCE=.\coolstuff.h
# End Source File
# Begin Source File

SOURCE=.\dialog.h
# End Source File
# Begin Source File

SOURCE=.\dib.h
# End Source File
# Begin Source File

SOURCE=.\entityed.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=.\importchrdlg.h
# End Source File
# Begin Source File

SOURCE=.\importvspdlg.h
# End Source File
# Begin Source File

SOURCE=.\layerdlg.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\mapdlg.h
# End Source File
# Begin Source File

SOURCE=.\mapview.h
# End Source File
# Begin Source File

SOURCE=.\miscdlg.h
# End Source File
# Begin Source File

SOURCE=.\pix_edit.h
# End Source File
# Begin Source File

SOURCE=.\tileed.h
# End Source File
# Begin Source File

SOURCE=.\tilesel.h
# End Source File
# Begin Source File

SOURCE=.\winmaped.h
# End Source File
# Begin Source File

SOURCE=.\zoneed.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\v26.ico
# End Source File
# Begin Source File

SOURCE=.\winmaped.rc
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

SOURCE=..\common\vector.h
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
# End Target
# End Project
