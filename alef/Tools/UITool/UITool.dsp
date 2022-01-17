# Microsoft Developer Studio Project File - Name="UITool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=UITool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UITool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UITool.mak" CFG="UITool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UITool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UITool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Alef/Tools/UITool", WWVBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UITool - Win32 Release"

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
# ADD LINK32 rtfsyst.lib rttiff.lib rtwing.lib rtgcond.lib rtimport.lib RtPITexD.lib mss32.lib rpmatfx.lib rplodatm.lib rpmorph.lib rtanim.lib rpspline.lib rtquat.lib rpskinmatfx.lib rttiff.lib rtras.lib rphanim.lib rpusrdat.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib AgcSkeleton_MFC_R.lib rtray.lib libfreetype.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /nodefaultlib:"LIBCP" /nodefaultlib:"LIBCPD" /nodefaultlib:"LIBMT" /nodefaultlib:"LIBMTD" /nodefaultlib:"MSVCRTD" /out:"../../Bin/UITool.exe"

!ELSEIF  "$(CFG)" == "UITool - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "RWDEBUG" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 rtfsyst.lib rttiff.lib rtwing.lib rtgcond.lib rtimport.lib RtPITexD.lib mss32.lib rpmatfx.lib rplodatm.lib rpmorph.lib rtanim.lib rpspline.lib rtquat.lib rpskinmatfx.lib rttiff.lib rtras.lib rphanim.lib rpusrdat.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib AgcSkeleton_MFC_D.lib rtray.lib libfreetype.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /nodefaultlib:"LIBCP" /nodefaultlib:"LIBCPD" /nodefaultlib:"d3dx9.lib" /out:"..\..\Bin\UIToolD.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "UITool - Win32 Release"
# Name "UITool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UIListView.cpp
# End Source File
# Begin Source File

SOURCE=.\UITool.cpp
# End Source File
# Begin Source File

SOURCE=.\UITool.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UIListView.h
# End Source File
# Begin Source File

SOURCE=.\UITool.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\UITool.ico
# End Source File
# Begin Source File

SOURCE=.\res\UITool.rc2
# End Source File
# End Group
# Begin Group "Renderware"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RenderWare.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWare.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MyEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\MyEngine.h
# End Source File
# End Group
# Begin Group "Window"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# End Group
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UITActionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITActionDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITCustomControlDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITCustomControlDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITDisplayMapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITDisplayMapDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITEventMapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITEventMapDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITHotkeyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITHotkeyDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITMessageMapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITMessageMapDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITModeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITModeDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\UITPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\UITStatusDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITStatusDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITUserDataDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITUserDataDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWindowProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWindowProperties.h
# End Source File
# Begin Source File

SOURCE=.\UITWinPropertyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWinPropertyDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPBarDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPBarDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPButtonDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPButtonDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPClockDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPClockDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPComboDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPComboDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPEditDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPEditDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPGridDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPGridDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPListDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPMapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPMapDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPScroll.h
# End Source File
# Begin Source File

SOURCE=.\UITWPSkillTreeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPSkillTreeDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPStyleDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPStyleDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPTreeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPTreeDialog.h
# End Source File
# Begin Source File

SOURCE=.\UITWPUIDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UITWPUIDialog.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
