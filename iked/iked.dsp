# Microsoft Developer Studio Project File - Name="iked" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=iked - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iked.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iked.mak" CFG="iked - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iked - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "iked - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iked - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "iked___Win32_Release"
# PROP BASE Intermediate_Dir "iked___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "\ika\common" /I "\ika\3rdparty" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "..\common" /I "..\3rdparty" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"../../lib" /libpath:"../../contrib/lib" /libpath:"\ika\common" /libpath:"\ika\3rdparty"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"../common" /libpath:"../3rdparty"

!ELSEIF  "$(CFG)" == "iked - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "iked___Win32_Debug"
# PROP BASE Intermediate_Dir "iked___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\ika\common" /I "\ika\3rdparty" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\3rdparty" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"../../lib" /libpath:"../../contrib/lib" /libpath:"\ika\common" /libpath:"\ika\3rdparty"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"../common" /libpath:"../3rdparty"

!ENDIF 

# Begin Target

# Name "iked - Win32 Release"
# Name "iked - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\codeview.cpp
# End Source File
# Begin Source File

SOURCE=.\configdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\mapview.cpp
# End Source File
# Begin Source File

SOURCE=.\projectview.cpp
# End Source File
# Begin Source File

SOURCE=.\tileset.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\codeview.h
# End Source File
# Begin Source File

SOURCE=.\configdlg.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\mapview.h
# End Source File
# Begin Source File

SOURCE=.\projectview.h
# End Source File
# Begin Source File

SOURCE=.\tileset.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\configfile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\configfile.h
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\fileio.h
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\log.h
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\misc.h
# End Source File
# Begin Source File

SOURCE=..\common\pixel_matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\common\rle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ika\common\types.h
# End Source File
# Begin Source File

SOURCE=..\common\vsp.cpp
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dlg.wxr
# End Source File
# Begin Source File

SOURCE=.\font.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\iked.rc

!IF  "$(CFG)" == "iked - Win32 Release"

!ELSEIF  "$(CFG)" == "iked - Win32 Debug"

# ADD BASE RSC /l 0x1009
# ADD RSC /l 0x1009
# SUBTRACT RSC /i "\wx2\include\msw\wx.rc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\map.ico
# End Source File
# Begin Source File

SOURCE=.\py.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\vsp.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\colours.bmp
# End Source File
# Begin Source File

SOURCE=.\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cross.bmp
# End Source File
# Begin Source File

SOURCE=.\wx\msw\csquery.bmp
# End Source File
# Begin Source File

SOURCE=.\wx\msw\disable.bmp
# End Source File
# Begin Source File

SOURCE=.\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\tick.bmp
# End Source File
# Begin Source File

SOURCE=.\wx\msw\tip.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\watch1.cur
# End Source File
# End Target
# End Project
