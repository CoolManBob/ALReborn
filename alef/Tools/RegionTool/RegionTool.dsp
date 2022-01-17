# Microsoft Developer Studio Project File - Name="RegionTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RegionTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RegionTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RegionTool.mak" CFG="RegionTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RegionTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RegionTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Alef/Tools/RegionTool", XHRDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RegionTool - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AuResourceKeeper.lib mpr.lib /nologo /subsystem:windows /pdb:"../../Bin/Release/RegionTool.pdb" /debug /machine:I386 /out:"../../Bin/Release/RegionTool.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "RegionTool - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AuResourceKeeper.lib mpr.lib /nologo /subsystem:windows /pdb:"../../Bin/RegionTool.pdb" /debug /machine:I386 /out:"../../Bin/RegionTool.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "RegionTool - Win32 Release"
# Name "RegionTool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\EventNatureSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GoToDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionTool.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionViewStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TemplateEditDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\EventNatureSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\GoToDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegionTool.h
# End Source File
# Begin Source File

SOURCE=.\RegionToolDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegionViewStatic.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TemplateEditDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\RegionTool.ico
# End Source File
# Begin Source File

SOURCE=.\RegionTool.rc
# End Source File
# Begin Source File

SOURCE=.\res\RegionTool.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RegionMenuDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionMenuDlg.h
# End Source File
# Begin Source File

SOURCE=.\RTMenuRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\RTMenuRegion.h
# End Source File
# Begin Source File

SOURCE=.\RTMenuSky.cpp
# End Source File
# Begin Source File

SOURCE=.\RTMenuSky.h
# End Source File
# Begin Source File

SOURCE=.\RTMenuSpawn.cpp
# End Source File
# Begin Source File

SOURCE=.\RTMenuSpawn.h
# End Source File
# End Group
# Begin Group "BugSlay"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BugSlay\BSUFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\BugslayerUtil.h
# End Source File
# Begin Source File

SOURCE=.\BugSlay\CrashHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\CrashHandler.h
# End Source File
# Begin Source File

SOURCE=.\BugSlay\GetLoadedModules.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\Internal.h
# End Source File
# Begin Source File

SOURCE=.\BugSlay\IsNT.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\NT4ProcessInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\PCH.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\PCH.h
# End Source File
# Begin Source File

SOURCE=.\BugSlay\SymbolEngine.h
# End Source File
# Begin Source File

SOURCE=.\BugSlay\TLHELPProcessInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\BugSlay\WarningsOff.h
# End Source File
# Begin Source File

SOURCE=.\BugSlay\WarningsOn.h
# End Source File
# End Group
# Begin Group "Path Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PathDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PathDialog.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
