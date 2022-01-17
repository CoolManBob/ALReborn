# Microsoft Developer Studio Project File - Name="MapTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MapTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MapTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MapTool.mak" CFG="MapTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MapTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MapTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Alef/Tools/MapTool", HTBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MapTool - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "C:\ALEF\RW\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AgcSkeleton_MFC_R.lib rtfsyst.lib RtRay.lib RtRAS.lib RpSpline.lib RpMorph.lib RtWing.lib RtGCond.lib RpLODAtm.lib RtPITexD.lib RtQuat.lib RpSkin.lib RtAnim.lib RpHAnim.lib rpusrdat.lib rpmatfx.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rttiff.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib rtray.lib libfreetype.lib /nologo /subsystem:windows /pdb:"../../Bin/Release/MapTool.pdb" /map /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcp" /nodefaultlib:"ApBaseD" /nodefaultlib:"AuIniManagerD" /nodefaultlib:"ApMemoryD" /nodefaultlib:"AuProfileManagerD" /nodefaultlib:"ApModuleD" /nodefaultlib:"AuPacketD" /nodefaultlib:"ApAdminD" /nodefaultlib:"ApmMapD" /nodefaultlib:"AgpmFactorsD" /nodefaultlib:"AuExcelTxtLibD" /nodefaultlib:"AgpmCharacterD" /nodefaultlib:"ApmOcTreeD" /nodefaultlib:"ApmObjectD" /nodefaultlib:"AgpmGridD" /nodefaultlib:"AuRandomNumberGeneratorD" /nodefaultlib:"AgpmItemD" /nodefaultlib:"AgpmUnionD" /nodefaultlib:"AgpmPartyD" /nodefaultlib:"AuGenerateIDD" /nodefaultlib:"ApmEventManagerD" /nodefaultlib:"AgpmEventProductD" /nodefaultlib:"AcClientSocketD" /nodefaultlib:"AgcModuleD" /nodefaultlib:"AgcmConnectManagerD" /nodefaultlib:"AcuObjectD" /nodefaultlib:"AgcmResourceLoaderD" /nodefaultlib:"AcuFrameMemoryD" /nodefaultlib:"AcuIMDrawD" /nodefaultlib:"AgcmOcTreeD" /nodefaultlib:"AgcmRenderD" /nodefaultlib:"AgcmMapD" /nodefaultlib:"AgcmAnimationD" /nodefaultlib:"AgcmShadowD" /nodefaultlib:"AgcmShadow2D" /nodefaultlib:"AgcmLODManagerD" /nodefaultlib:"AgcmFontD" /nodefaultlib:"AgcmSoundD" /nodefaultlib:"AuTickCounterD" /nodefaultlib:"AcuTextureD" /nodefaultlib:"AgcmUIControlD" /nodefaultlib:"AgcmObjectD" /nodefaultlib:"AgcmCharacterD" /nodefaultlib:"AgcmEventManagerD" /nodefaultlib:"AgcmEventProductD" /nodefaultlib:"AgpmItemConvertD" /nodefaultlib:"AgpmDropItemD" /nodefaultlib:"AgcmItemD" /nodefaultlib:"AgpmTimerD" /nodefaultlib:"AgpmEventNatureD" /nodefaultlib:"AgcmEffectD" /nodefaultlib:"AgcmEventNatureD" /nodefaultlib:"AgpmEventSpawnD" /nodefaultlib:"AgpmEventTeleportD" /nodefaultlib:"AgpmEventNPCDialogD" /nodefaultlib:"AgpmEventNPCTradeD" /nodefaultlib:"AgcmEventNPCTradeD" /nodefaultlib:"ApmEventManagerDlgD" /nodefaultlib:"AgcmEventNPCDialogDlgD" /nodefaultlib:"AgcmEventNPCTradeDlgD" /nodefaultlib:"AgcmFactorsDlgD" /nodefaultlib:"AgpmSkillD" /nodefaultlib:"AgcmSkillD" /nodefaultlib:"AgpmPathFindD" /nodefaultlib:"AgpmAI2D" /nodefaultlib:"AgcmAI2DlgD" /nodefaultlib:"AgcmEventSpawnDlgD" /nodefaultlib:"AgcmEventTeleportDlgD" /nodefaultlib:"AgpmAID" /nodefaultlib:"AgcmCameraD" /nodefaultlib:"AgcmWaterD" /nodefaultlib:"AgcaEffectDataD" /nodefaultlib:"AgcmEventEffectD" /nodefaultlib:"AgpmShrineD" /nodefaultlib:"AgcmShrineDlgD" /nodefaultlib:"AgcmEventNatureDlgD" /nodefaultlib:"AgcmBlockingDlgD" /nodefaultlib:"AgcmPreLODManagerD" /nodefaultlib:"AgcmLODDlgD" /nodefaultlib:"AgcmEffectDlgD" /nodefaultlib:"agcmAnimationDlgD" /nodefaultlib:"AgcmSkillDlgD" /nodefaultlib:"AgcmFileListDlgD" /nodefaultlib:"AgcmWaterDlgD" /nodefaultlib:"AgcmLensFlareD" /nodefaultlib:"AgcmTextBoardD" /nodefaultlib:"AgcmTunerD" /nodefaultlib:"AgcmObjectTypeDlgD" /nodefaultlib:"AgpmEventMasterySpecializeD" /nodefaultlib:"AgcmGrassD" /nodefaultlib:"AgcmDynamicLightmapD" /nodefaultlib:"AgpmEventBankD" /nodefaultlib:"AgcmEventBankD" /nodefaultlib:"AgpmEventItemConvertD" /nodefaultlib:"AgcmEventItemConvertD" /nodefaultlib:"AgpmEventGuildD" /nodefaultlib:"AgcmEventGuildD" /nodefaultlib:"AcuRpMTextureD" /nodefaultlib:"AcuRpUVAnimDataD" /nodefaultlib:"d3dx9.lib" /out:"../../Bin/Release/MapTool.exe" /libpath:"C:\ALEF\RW\LIB\RELEASE"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MapTool - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\ALEF\RW\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWDEBUG" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AgcSkeleton_MFC_D.lib rtfsyst.lib RtRay.lib RtRAS.lib RpSpline.lib RpMorph.lib RtWing.lib RtGCond.lib RpLODAtm.lib RtPITexD.lib RtQuat.lib RpSkin.lib RtAnim.lib RpHAnim.lib rpusrdat.lib rpmatfx.lib rtimport.lib rtintsec.lib rtpick.lib rpcollis.lib rtcharse.lib rtbmp.lib rtpng.lib rttiff.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib rtray.lib rtdict.lib rpuvanim.lib libfreetype.lib /nologo /subsystem:windows /pdb:"C:\ALEF\bin\MapTool_Debug.pdb" /map /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /nodefaultlib:"libcp" /nodefaultlib:"libcpd" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"nafxcwd" /nodefaultlib:"d3dx9.lib" /out:"../../bin/MapTool_Debug.exe" /libpath:"C:\ALEF\RW\LIB\DEBUG"
# SUBTRACT LINK32 /profile /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Map 파일 카피	copy debug\MapTool_debug.map ..\..\bin\MapTool_Debug.map
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "MapTool - Win32 Release"
# Name "MapTool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AdjustHeightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AlefMapDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\AlefToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\BobbyUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\ClipPlaneSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConstManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertTextureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DDSConvertDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventNatureChangeFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GrassSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MapSelectStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\MapTool.cpp
# End Source File
# Begin Source File

SOURCE=.\NPCNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectBrushSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWare.cpp
# End Source File
# Begin Source File

SOURCE=.\RgbSelectStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UIMiniMapWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UIOutputWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileList_MapTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileList_ObjectTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileList_TileTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileListWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterHeightDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AdjustHeightDlg.h
# End Source File
# Begin Source File

SOURCE=.\AlefMapDocument.h
# End Source File
# Begin Source File

SOURCE=.\AlefToolBar.h
# End Source File
# Begin Source File

SOURCE=.\BobbyUtil.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\ClipPlaneSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConstManager.h
# End Source File
# Begin Source File

SOURCE=.\ConvertTextureDlg.h
# End Source File
# Begin Source File

SOURCE=.\DDSConvertDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventNatureChangeFilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\GrassSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\HtmlWnd.h
# End Source File
# Begin Source File

SOURCE=.\KeyReferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MapSelectStatic.h
# End Source File
# Begin Source File

SOURCE=.\MapTool.h
# End Source File
# Begin Source File

SOURCE=.\NPCNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\ObjectBrushSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\RenderWare.h
# End Source File
# Begin Source File

SOURCE=.\RgbSelectStatic.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UIMiniMapWnd.h
# End Source File
# Begin Source File

SOURCE=.\UIOutputWnd.h
# End Source File
# Begin Source File

SOURCE=.\UITileList_MapTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\UITileList_ObjectTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\UITileList_TileTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\UITileListWnd.h
# End Source File
# Begin Source File

SOURCE=.\WaterHeightDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\alpha.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ambient.ico
# End Source File
# Begin Source File

SOURCE=.\res\anim.ico
# End Source File
# Begin Source File

SOURCE=.\res\background.bmp
# End Source File
# Begin Source File

SOURCE=.\res\backgroundforEvent.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Blocking_Plugin.bmp
# End Source File
# Begin Source File

SOURCE=.\res\camera.ico
# End Source File
# Begin Source File

SOURCE=.\res\clump.ico
# End Source File
# Begin Source File

SOURCE=.\res\color.bmp
# End Source File
# Begin Source File

SOURCE=.\res\colortable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\directional.ico
# End Source File
# Begin Source File

SOURCE=.\res\hsv_tabl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HSV_Table.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MapTool.ico
# End Source File
# Begin Source File

SOURCE=.\res\MapTool.rc2
# End Source File
# Begin Source File

SOURCE=.\res\snapto.ico
# End Source File
# Begin Source File

SOURCE=.\res\softspot.ico
# End Source File
# Begin Source File

SOURCE=.\res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\spotlight.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBAT_ICON_DOWN.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBAT_ICON_UP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\trash.ico
# End Source File
# Begin Source File

SOURCE=.\res\world.ico
# End Source File
# Begin Source File

SOURCE=.\res\zdolly.bmp
# End Source File
# Begin Source File

SOURCE=".\res\각각알파올려놓기 셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\노멀셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\배경찍기.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\섭디비전셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\섭디비전언셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\알파셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\오브젝트셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\오브젝트언셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\지오메트리셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\지오메트리언셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일링셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일선택이미지.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일선택이미지2.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일언셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일창타일.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일카테고리셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\타일카테고리언셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\통짜셀렉트.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\플로트셀렉트.bmp"
# End Source File
# End Group
# Begin Group "EngineClass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MyEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\MyEngine.h
# End Source File
# End Group
# Begin Group "MainWindow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\MapToolConst.h
# End Source File
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HeightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightMap.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MapTool.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AtomicFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AtomicFilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConstDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConstDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditConsoleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditConsoleDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\HSVControlStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\HSVControlStatic.h
# End Source File
# Begin Source File

SOURCE=.\MapSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MapSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\ObjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectDialog.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionDialog.h
# End Source File
# Begin Source File

SOURCE=.\PortionStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\PortionStatic.h
# End Source File
# Begin Source File

SOURCE=.\PrelightAdjustDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrelightAdjustDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\RgbSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RgbSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\RoughMapGenerateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RoughMapGenerateDlg.h
# End Source File
# Begin Source File

SOURCE=.\SaveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveDlg.h
# End Source File
# Begin Source File

SOURCE=.\SectorCreateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SectorCreateDlg.h
# End Source File
# Begin Source File

SOURCE=.\SkyTemplatePreviewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SkyTemplatePreviewDlg.h
# End Source File
# Begin Source File

SOURCE=.\SkyValueChangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SkyValueChangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\SplashDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashDlg.h
# End Source File
# Begin Source File

SOURCE=.\SubDivisionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SubDivisionDlg.h
# End Source File
# Begin Source File

SOURCE=.\TileEditCategoryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TileEditCategoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\TileNameEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TileNameEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\UITileList_EditTileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileList_EditTileDlg.h
# End Source File
# End Group
# Begin Group "Undo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoManager.h
# End Source File
# End Group
# Begin Group "Plugins"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Plugin_BitmapExport.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_BitmapExport.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_Blocking.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_Blocking.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_CameraWalk.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_CameraWalk.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_Grass.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_Grass.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_NPC.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_NPC.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_TextureInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_TextureInfo.h
# End Source File
# Begin Source File

SOURCE=.\Plugin_Water.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin_Water.h
# End Source File
# Begin Source File

SOURCE=.\UITileList_Others.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileList_Others.h
# End Source File
# Begin Source File

SOURCE=.\UITileList_PluginBase.cpp
# End Source File
# Begin Source File

SOURCE=.\UITileList_PluginBase.h
# End Source File
# End Group
# Begin Group "BugSlayer"

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
# Begin Group "DIBLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DIBAPI.H
# End Source File
# Begin Source File

SOURCE=.\DIBUTIL.CPP
# End Source File
# Begin Source File

SOURCE=.\DIBUTIL.H
# End Source File
# End Group
# Begin Group "Dungeon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DungeonTemplateEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DungeonTemplateEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\DungeonToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DungeonToolBar.h
# End Source File
# Begin Source File

SOURCE=.\DungeonWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\DungeonWnd.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
