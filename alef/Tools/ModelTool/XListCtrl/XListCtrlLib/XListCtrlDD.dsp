# Microsoft Developer Studio Project File - Name="XListCtrlDD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XListCtrlDD - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XListCtrlDD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XListCtrlDD.mak" CFG="XListCtrlDD - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XListCtrlDD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XListCtrlDD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XListCtrlDD - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XListCtrlDD - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XListCtrlDD - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release/XListCtrlDDRA.dll"
# Begin Special Build Tool
OutDir=.\Release
TargetName=XListCtrlDDRA
SOURCE="$(InputPath)"
PostBuild_Desc=Copying files...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).dll ..\bin 1> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XListCtrlDD - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/XListCtrlDDDA.dll" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\Debug
TargetName=XListCtrlDDDA
SOURCE="$(InputPath)"
PostBuild_Desc=Copying files...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).dll ..\bin 1> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XListCtrlDD - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Unicode"
# PROP BASE Intermediate_Dir "Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/XListCtrlD.dll" /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug_Unicode/XListCtrlDDDU.dll" /pdbtype:sept
# Begin Special Build Tool
OutDir=.\Debug_Unicode
TargetName=XListCtrlDDDU
SOURCE="$(InputPath)"
PostBuild_Desc=Copying files...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).dll ..\bin 1> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XListCtrlDD - Win32 Release Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Unicode"
# PROP BASE Intermediate_Dir "Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release/XListCtrlR.dll"
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release_Unicode/XListCtrlDDRU.dll"
# Begin Special Build Tool
OutDir=.\Release_Unicode
TargetName=XListCtrlDDRU
SOURCE="$(InputPath)"
PostBuild_Desc=Copying files...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).dll ..\bin 1> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "XListCtrlDD - Win32 Release"
# Name "XListCtrlDD - Win32 Debug"
# Name "XListCtrlDD - Win32 Debug Unicode"
# Name "XListCtrlDD - Win32 Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\XListCtrl\AdvComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\AdvComboEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\DropListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\DropScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\DropWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\VisualStylesXP.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XCombo.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XComboList.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XListCtrlDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\XListCtrlDLL.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\XListCtrl\AdvComboBox.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\AdvComboEdit.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\DropListBox.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\DropScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\DropWnd.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\SortCStringArray.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\VisualStylesXP.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XCombo.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XComboList.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XEdit.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\XListCtrl\XTrace.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
