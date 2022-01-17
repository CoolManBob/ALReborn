# Microsoft Developer Studio Project File - Name="ModelTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ModelTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ModelTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ModelTool.mak" CFG="ModelTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ModelTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ModelTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Alef/Tools/ModelTool", XJYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ModelTool - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 libfreetype.lib /nologo /subsystem:windows /machine:I386 /out:"../../Bin/ModelTool.exe"

!ELSEIF  "$(CFG)" == "ModelTool - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWDEBUG" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mpr.lib mss32.lib AgcSkeleton_MFC_D.lib AuResourceKeeper.lib rtfsyst.lib rtray.lib rplodatm.lib rtanim.lib rpspline.lib rtquat.lib rpskinmatfx.lib rttiff.lib rtras.lib rphanim.lib rpusrdat.lib rtgcond.lib rtwing.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rpworld.lib rwcore.lib rpmatfx.lib rtpitexd.lib rpmorph.lib winmm.lib randomam.lib libfreetype.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /nodefaultlib:"LIBCP" /nodefaultlib:"LIBCPD" /nodefaultlib:"LIBCMT" /nodefaultlib:"LIBCMTD" /nodefaultlib:"nafxcwd" /nodefaultlib:"d3dx9" /out:"..\..\BIN\ModelTool.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ModelTool - Win32 Release"
# Name "ModelTool - Win32 Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "AMT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AMT_AddObject.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Animation.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Blocking.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_EditEquipments.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Initialize.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_LOD.CPP
# End Source File
# Begin Source File

SOURCE=.\AMT_Merge.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_OpenDetailInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_PropertyFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Release.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Render.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_ResourceKeeper.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Save.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_SetObject.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Test.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_ToolFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\AMT_Update.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AgcEngineChild.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AgcEngineChild.h
# End Source File
# Begin Source File

SOURCE=.\ModelTool.h
# End Source File
# Begin Source File

SOURCE=.\ModelToolDlg.h
# End Source File
# Begin Source File

SOURCE=..\ResourceManager\RM_Define.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolDefine.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\arrowcop.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\MenuToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MenuToolBar2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ModelTool.ico
# End Source File
# Begin Source File

SOURCE=.\nodrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\ResourceToolbar.bmp
# End Source File
# End Group
# Begin Group "Rsc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ModelTool.rc
# End Source File
# Begin Source File

SOURCE=.\res\ModelTool.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# End Group
# Begin Group "MFC class"

# PROP Default_Filter ""
# Begin Group "Attribute"

# PROP Default_Filter ""
# Begin Group "Attribute_S"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventButton.cpp
# End Source File
# End Group
# Begin Group "Attribute_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventButton.h
# End Source File
# End Group
# End Group
# Begin Group "Custom"

# PROP Default_Filter ""
# Begin Group "Custom_S"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MenuForm.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyForm.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyTree.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderForm.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderView.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceTree.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx__.cpp
# End Source File
# End Group
# Begin Group "Custom_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MenuForm.h
# End Source File
# Begin Source File

SOURCE=.\PropertyForm.h
# End Source File
# Begin Source File

SOURCE=.\PropertyTree.h
# End Source File
# Begin Source File

SOURCE=.\RenderForm.h
# End Source File
# Begin Source File

SOURCE=.\RenderView.h
# End Source File
# Begin Source File

SOURCE=.\ResourceForm.h
# End Source File
# Begin Source File

SOURCE=.\ResourceTree.h
# End Source File
# Begin Source File

SOURCE=.\TitleForm.h
# End Source File
# Begin Source File

SOURCE=.\ToolForm.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx__.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "RenderWare"

# PROP Default_Filter ""
# Begin Group "RW_S"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RenderWare.cpp
# End Source File
# Begin Source File

SOURCE=.\RWUtil.cpp
# End Source File
# End Group
# Begin Group "RW_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RenderWare.h
# End Source File
# Begin Source File

SOURCE=.\RWUtil.h
# End Source File
# End Group
# End Group
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Group "Dialog_S"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateObjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DefaultEquipmentsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEquipmentsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemTemplateListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LockObjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OffsetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UploadAllRKDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UploadResourceKeeperDlg.cpp
# End Source File
# End Group
# Begin Group "Dialog_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimationDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DefaultEquipmentsDlg.h
# End Source File
# Begin Source File

SOURCE=.\DetailInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditEquipmentsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ItemTemplateListDlg.h
# End Source File
# Begin Source File

SOURCE=.\LockObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\MergeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\OffsetDlg.h
# End Source File
# Begin Source File

SOURCE=.\RenderDlg.h
# End Source File
# Begin Source File

SOURCE=.\SaveDlg.h
# End Source File
# Begin Source File

SOURCE=.\StringDlg.h
# End Source File
# Begin Source File

SOURCE=.\TreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\UploadAllRKDlg.h
# End Source File
# Begin Source File

SOURCE=.\UploadResourceKeeperDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\LockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LockDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
