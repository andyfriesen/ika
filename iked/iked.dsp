# Microsoft Developer Studio Project File - Name="iked" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=iked - Win32 Debug wx232
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iked.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iked.mak" CFG="iked - Win32 Debug wx232"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iked - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "iked - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "iked - Win32 Debug wx232" (based on "Win32 (x86) Application")
!MESSAGE "iked - Win32 Release wx232" (based on "Win32 (x86) Application")
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
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "..\common" /I "..\3rdparty" /I "$(WXWIN)\include" /I "$(WXWIN)\contrib\include" /D "NDEBUG" /D "MSVC" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx.lib xpm.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"..\common" /libpath:"..\3rdparty" /libpath:"$(WXWIN)\lib" /libpath:"$(WXWIN)\contrib\lib"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\3rdparty" /I "$(WXWIN)\include" /I "$(WXWIN)\contrib\include" /D "_DEBUG" /D DEBUG=1 /D "__WXDEBUG__" /D "MSVC" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"../../lib" /libpath:"../../contrib/lib" /libpath:"\ika\common" /libpath:"\ika\3rdparty"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxd.lib xpmd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"..\common" /libpath:"..\3rdparty" /libpath:"$(WXWIN)\lib" /libpath:"$(WXWIN)\contrib\lib"

!ELSEIF  "$(CFG)" == "iked - Win32 Debug wx232"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "iked___Win32_Debug_wx232"
# PROP BASE Intermediate_Dir "iked___Win32_Debug_wx232"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_wx232"
# PROP Intermediate_Dir "Debug_wx232"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\3rdparty" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\3rdparty" /I "\wx232\include" /I "\wx232\contrib\include" /D "_DEBUG" /D DEBUG=1 /D "__WXDEBUG__" /D "WX232" /D "MSVC" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "_DEBUG"
# ADD RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"../common" /libpath:"../3rdparty"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"..\common" /libpath:"..\3rdparty" /libpath:"\wx232\lib" /libpath:"\wx232\contrib\lib"

!ELSEIF  "$(CFG)" == "iked - Win32 Release wx232"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "iked___Win32_Release_wx232"
# PROP BASE Intermediate_Dir "iked___Win32_Release_wx232"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_wx232"
# PROP Intermediate_Dir "Release_wx232"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "..\common" /I "..\3rdparty" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "..\common" /I "..\3rdparty" /I "\wx232\include" /I "\wx232\contrib\include" /D "NDEBUG" /D "WX232" /D "MSVC" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "NDEBUG"
# ADD RSC /l 0x809 /i "\wx2\include\msw\wx.rc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"../common" /libpath:"../3rdparty"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"..\common" /libpath:"..\3rdparty" /libpath:"\wx232\lib" /libpath:"\wx232\contrib\lib"

!ENDIF 

# Begin Target

# Name "iked - Win32 Release"
# Name "iked - Win32 Debug"
# Name "iked - Win32 Debug wx232"
# Name "iked - Win32 Release wx232"
# Begin Group "Source"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\codeview.cpp
# End Source File
# Begin Source File

SOURCE=.\configdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\docview.cpp
# End Source File
# Begin Source File

SOURCE=.\fontview.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\layervisibilitycontrol.cpp
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
# Begin Source File

SOURCE=.\tilesetview.cpp
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

SOURCE=.\controller.h
# End Source File
# Begin Source File

SOURCE=.\docview.h
# End Source File
# Begin Source File

SOURCE=.\fontview.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=.\layervisibilitycontrol.h
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
# Begin Source File

SOURCE=.\tilesetview.h
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

SOURCE=..\common\pixel_matrix_blitter.h
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

SOURCE=..\common\vsp.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vsp.h
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

!ELSEIF  "$(CFG)" == "iked - Win32 Debug wx232"

# ADD BASE RSC /l 0x1009
# SUBTRACT BASE RSC /i "\wx2\include\msw\wx.rc"
# ADD RSC /l 0x1009
# SUBTRACT RSC /i "\wx2\include\msw\wx.rc"

!ELSEIF  "$(CFG)" == "iked - Win32 Release wx232"

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
# End Target
# End Project
