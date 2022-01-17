# Microsoft Developer Studio Project File - Name="EffTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EffTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EffTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EffTool.mak" CFG="EffTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EffTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "EffTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ALEF/Tools/EffTool", SRYXAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EffTool - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "EffTool - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "C:\Program Files\Codejock Software\MFC\ToolkitPro\include\PropertyGrid" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWDEBUG" /D "_AFXDLL" /D "EFF2_SHOWINFO" /D "EFF2_SHOWLOG" /D "EFF2_SHOWERR" /D "_CALCBOUNDINFO" /D "EFF2_FORTOOL" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 rtfsyst.lib rplodatm.lib rtpitexd.lib mss32.lib rtanim.lib AgcSkeleton_MFC_D.lib RpUsrDat.lib rpspline.lib rpskin.lib rphanim.lib rpmorph.lib rtquat.lib rpmatfx.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rttiff.lib rtras.lib rpworld.lib rplogo.lib winmm.lib rtgcond.lib rtwing.lib rtray.lib rwcore.lib AgcmEff2D.lib libfreetype.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /nodefaultlib:"libcpd" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcpmt" /nodefaultlib:"libcpmtd" /nodefaultlib:"d3dx9.lib" /out:".\Debug\EffToolD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "EffTool - Win32 Release"
# Name "EffTool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrowserView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\EdtView.cpp
# End Source File
# Begin Source File

SOURCE=.\EffTool.cpp
# End Source File
# Begin Source File

SOURCE=.\EffTool.rc
# End Source File
# Begin Source File

SOURCE=.\EffToolDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\EffToolView.cpp
# End Source File
# Begin Source File

SOURCE=.\FrmView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ToolOption.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BrowserView.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\EdtView.h
# End Source File
# Begin Source File

SOURCE=.\EffTool.h
# End Source File
# Begin Source File

SOURCE=.\EffToolDoc.h
# End Source File
# Begin Source File

SOURCE=.\EffToolView.h
# End Source File
# Begin Source File

SOURCE=.\FrmView.h
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

SOURCE=.\ToolOption.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EffTool.ico
# End Source File
# Begin Source File

SOURCE=.\res\EffTool.rc2
# End Source File
# Begin Source File

SOURCE=.\res\EffToolDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Group "Rw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\RwCam\Cam.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\RwCam\Cam.h
# End Source File
# Begin Source File

SOURCE=.\Util\RW\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\RW\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\Util\RW\MyEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\RW\MyEngine.h
# End Source File
# Begin Source File

SOURCE=.\Util\RW\RenderWare.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\RW\RenderWare.h
# End Source File
# End Group
# Begin Group "visualUtil"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\VisualUtil\Arrow.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\Arrow.h
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\Axis.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\Axis.h
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\EffUt_Geo.h
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\EffUt_HFMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\EffUt_HFMap.h
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\EffUt_Vtx.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\EffUt_Vtx.h
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\VisualUtil\Grid.h
# End Source File
# End Group
# Begin Group "Pick"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\Pick\PickUser.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\Pick\PickUser.h
# End Source File
# End Group
# Begin Group "d3dwzrd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\D3dWizard\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\Util\D3dWizard\dxutil.h
# End Source File
# End Group
# Begin Group "bmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\bmpLoader\ksBmpLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\bmpLoader\ksBmpLoader.h
# End Source File
# End Group
# End Group
# Begin Group "XTPPropertyGridItemCustum"

# PROP Default_Filter ""
# Begin Group "src"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_EffSet.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_UINT.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPPropertyGridItemEnum.cpp
# End Source File
# End Group
# Begin Group "hdr"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_EffSet.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_UINT.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPPropertyGridItemEnum.h
# End Source File
# End Group
# Begin Group "EffBase"

# PROP Default_Filter ""
# Begin Group "hdr No. 3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Board.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Light.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_MFrm.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Obj.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_PSys.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_PSysSBH.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Sound.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Tail.h
# End Source File
# End Group
# Begin Group "src No. 3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Board.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Light.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_MFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Obj.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_PSys.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_PSysSBH.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Tail.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_EffBase.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_EffBase.h
# End Source File
# End Group
# Begin Group "EffAnim"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_AColr.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_AColr.h
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Anim.cpp
# End Source File
# Begin Source File

SOURCE=.\XTPPropertyGridItemCustum\XTPGI_Anim.h
# End Source File
# End Group
# End Group
# Begin Group "Dlg"

# PROP Default_Filter ""
# Begin Group "src No. 1"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Dlg\dlg_effbase.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\Dlg_EffRenderBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\dlg_newanimitem.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\dlg_neweffanim.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgEdit.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgListEffSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgListEffTex.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgNewEffBase.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgNewEffSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgStaticPropGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgStaticTexPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgTreeEffSet.cpp
# End Source File
# End Group
# Begin Group "hdr No. 1"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Dlg\dlg_effbase.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\Dlg_EffRenderBase.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\dlg_newanimitem.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\dlg_neweffanim.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgEdit.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgListEffSet.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgListEffTex.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\dlgneweffbase.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgNewEffSet.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgStaticPropGrid.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgStaticTexPreview.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgTreeEffSet.h
# End Source File
# End Group
# Begin Group "DlgBase"

# PROP Default_Filter ""
# Begin Group "src No. 2"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Dlg\DlgList.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgTree.cpp
# ADD CPP /I ".."
# End Source File
# End Group
# Begin Group "hdr No. 2"

# PROP Default_Filter "h;"
# Begin Source File

SOURCE=.\Dlg\DlgList.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgStatic.h
# End Source File
# Begin Source File

SOURCE=.\Dlg\DlgTree.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Dlg\dlg_texuvselect.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dlg\dlg_texuvselect.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
