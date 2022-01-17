# Microsoft Developer Studio Project File - Name="XListCtrlDS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XListCtrlDS - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XListCtrlDS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XListCtrlDS.mak" CFG="XListCtrlDS - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XListCtrlDS - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XListCtrlDS - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XListCtrlDS - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "XListCtrlDS - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XListCtrlDS - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "XLISTCTRLLIB_STATIC" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\XListCtrlDSRA.lib"
# Begin Special Build Tool
OutDir=.\Release
TargetName=XListCtrlDSRA
SOURCE="$(InputPath)"
PostBuild_Desc=Copying file...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XListCtrlDS - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "XLISTCTRLLIB_STATIC" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\XListCtrlDSDA.lib"
# Begin Special Build Tool
OutDir=.\Debug
TargetName=XListCtrlDSDA
SOURCE="$(InputPath)"
PostBuild_Desc=Copying file...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XListCtrlDS - Win32 Debug Unicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Unicode"
# PROP BASE Intermediate_Dir "Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "XLISTCTRLLIB_STATIC" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug_Unicode\XListCtrlDSDU.lib"
# Begin Special Build Tool
OutDir=.\Debug_Unicode
TargetName=XListCtrlDSDU
SOURCE="$(InputPath)"
PostBuild_Desc=Copying file...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XListCtrlDS - Win32 Release Unicode"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Unicode"
# PROP BASE Intermediate_Dir "Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "UNIOCODE" /D "WIN32" /D "_WINDOWS" /D "XLISTCTRLLIB_STATIC" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release_Unicode\XListCtrlDSRU.lib"
# Begin Special Build Tool
OutDir=.\Release_Unicode
TargetName=XListCtrlDSRU
SOURCE="$(InputPath)"
PostBuild_Desc=Copying file...
PostBuild_Cmds=mkdir ..\bin 2> nul	copy $(Outdir)\$(TargetName).lib ..\bin 1> nul
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "XListCtrlDS - Win32 Release"
# Name "XListCtrlDS - Win32 Debug"
# Name "XListCtrlDS - Win32 Debug Unicode"
# Name "XListCtrlDS - Win32 Release Unicode"
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
# End Target
# End Project
