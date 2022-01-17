# Microsoft Developer Studio Project File - Name="MatD3DFxTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MatD3DFxTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MatD3DFxTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MatD3DFxTool.mak" CFG="MatD3DFxTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MatD3DFxTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MatD3DFxTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ALEF/Tools/MatD3DFxTool", WHWEBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MatD3DFxTool - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /fo"Release/MatD3DFxTool.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx9.lib User32.lib Advapi32.lib Gdi32.lib rtfsyst.lib RtRay.lib RtRAS.lib RpSpline.lib RpMorph.lib RtWing.lib RtGCond.lib RpLODAtm.lib RtPITexD.lib RtQuat.lib RpSkin.lib RtAnim.lib RpHAnim.lib rpusrdat.lib rpmatfx.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rttiff.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib rtray.lib rtskinsp.lib AcuRpMatD3DFx.lib dxerr9.lib Comdlg32.lib dbghelp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:"../../Bin/MatD3DFxTool.pdb" /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /nodefaultlib:"libcp" /nodefaultlib:"libcpd" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"nafxcwd" /out:"../../Bin/MatD3DFxTool.exe" /libpath:"D:\ALEF\RW\lib\release" /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MatD3DFxTool - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "RWDEBUG" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /fo"Debug/MatD3DFxTool.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9.lib User32.lib Advapi32.lib Gdi32.lib dxerr9.lib rtfsyst.lib RtRay.lib RtRAS.lib RpSpline.lib RpMorph.lib RtWing.lib RtGCond.lib RpLODAtm.lib RtPITexD.lib RtQuat.lib RpSkin.lib RtAnim.lib RpHAnim.lib rpusrdat.lib rpmatfx.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rttiff.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib rtray.lib rtdict.lib rpuvanim.lib rtskinsp.lib AcuRpMatD3DFxD.lib Comdlg32.lib dbghelp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:"../../bin/MatD3DFxToolD.pdb" /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /nodefaultlib:"libcp" /nodefaultlib:"libcpd" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"nafxcwd" /out:"../../bin/MatD3DFxToolD.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MatD3DFxTool - Win32 Release"
# Name "MatD3DFxTool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\FXUI.cpp
# End Source File
# Begin Source File

SOURCE=.\MatD3DFxTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Control.h
# End Source File
# Begin Source File

SOURCE=.\FXUI.h
# End Source File
# Begin Source File

SOURCE=.\Scene.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\skel\main.ico
# End Source File
# Begin Source File

SOURCE=.\skel\win.rc
# End Source File
# End Group
# Begin Group "skel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skel\camera.c
# End Source File
# Begin Source File

SOURCE=.\skel\camera.h
# End Source File
# Begin Source File

SOURCE=.\skel\menu.c
# End Source File
# Begin Source File

SOURCE=.\skel\menu.h
# End Source File
# Begin Source File

SOURCE=.\skel\mouse.c
# End Source File
# Begin Source File

SOURCE=.\skel\mouse.h
# End Source File
# Begin Source File

SOURCE=.\skel\mousedat.h
# End Source File
# Begin Source File

SOURCE=.\skel\platform.h
# End Source File
# Begin Source File

SOURCE=.\skel\ptrdata.h
# End Source File
# Begin Source File

SOURCE=.\skel\resource.h
# End Source File
# Begin Source File

SOURCE=.\skel\skeleton.c
# End Source File
# Begin Source File

SOURCE=.\skel\skeleton.h
# End Source File
# Begin Source File

SOURCE=.\skel\win.c
# End Source File
# Begin Source File

SOURCE=.\skel\win.h
# End Source File
# End Group
# Begin Group "DXUTgui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DXUTgui\DXUTgui.cpp
# End Source File
# Begin Source File

SOURCE=.\DXUTgui\DXUTgui.h
# End Source File
# Begin Source File

SOURCE=.\DXUTgui\DXUTmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\DXUTgui\DXUTmisc.h
# End Source File
# End Group
# Begin Group "MemoryManager"

# PROP Default_Filter ".cpp;.h"
# Begin Source File

SOURCE=.\MemoryManager\mmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MemoryManager\mmgr.h
# End Source File
# Begin Source File

SOURCE=.\MemoryManager\nommgr.h
# End Source File
# End Group
# Begin Group "XCrashReport"

# PROP Default_Filter ".cpp;.h"
# Begin Source File

SOURCE=.\XCrashReport\CrashFileNames.h
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\dbghelp.h
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\ExceptionHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\ExceptionHandler.h
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\GetWinVer.cpp
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\GetWinVer.h
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\MiniVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\XCrashReport\MiniVersion.h
# End Source File
# End Group
# End Target
# End Project
