# Microsoft Developer Studio Project File - Name="WinXP.Royale" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WinXP.Royale - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinXP.Royale.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinXP.Royale.mak" CFG="WinXP.Royale - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinXP.Royale - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release/vc60"
# PROP BASE Intermediate_Dir "Release/vc60"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release/vc60"
# PROP Intermediate_Dir "Release/vc60"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XTPRESOURCEARSA_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../../../Source" /I "../../../Source/Common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XTPRESOURCE_EXPORTS" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /nodefaultlib /out:"../WinXP.Royale.cjstyles" /opt:nowin98 /noentry
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "WinXP.Royale - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\WinXP.Royale.rc
# End Source File
# End Group
# Begin Group "Image Files"

# PROP Default_Filter "bmp"
# Begin Source File

SOURCE=.\res\Royale_Button.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CheckBox13.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CheckBox16.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CheckBox25.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_Chevron.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CloseGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CloseGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CloseGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_CloseGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ComboButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ComboButtonGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ExplorerBarHeaderClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ExplorerBarHeaderPin.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FieldOutlineBlue.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameBottom.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameCaptionMin.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameMaximized.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_FrameRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_Gripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_GripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_GroupBox.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_HelpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_HelpGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_HelpGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_HelpGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_HelpGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ListViewHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ListviewHeaderBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MaximizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MaximizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MaximizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MaximizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MaximizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MDICaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MDIGlyphClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MDIGlyphMinimize.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MDIGlyphRestore.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MinimizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MinimizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MinimizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MinimizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_MinimizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_NormalGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_NormalGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_NormalGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_NormalGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_PlaceBarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_PlaceBarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ProgressChunk.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ProgressChunkVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ProgressTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ProgressTrackVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RadioButton13.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RadioButton16.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RadioButton25.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ResizeGrip2.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RestoreGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RestoreGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RestoreGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RestoreGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_RestoreGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollArrowGlyphs.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollArrowGlyphsSmall.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollArrows.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollShaftHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollShaftVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollThumbGripperHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollThumbGripperVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollThumbHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ScrollThumbVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_Separator.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SeparatorVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SliderTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SmallCloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SmallCloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SmallFrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SmallFrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpecialGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpecialGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpecialGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpecialGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinButtonBackgroundDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinButtonBackgroundLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinButtonBackgroundRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinButtonBackgroundUp.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinLeftGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinRightGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_SpinUpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_StatusBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_StatusPane.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabBackground133.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItem.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemTop.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemTopBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemTopLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabItemTopRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TabPaneEdge.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarButtonsSplit.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarButtonsSplitDropdown.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarButtonsSplitDropdownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_ToolbarGripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarDown13.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarDown16.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarDown25.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackbarHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarLeft13.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarLeft16.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarLeft25.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackbarRight13.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarRight16.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarRight25.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarUp13.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarUp16.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackBarUp25.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TrackbarVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TreeExpandCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TreeExpandCollapse10.png
# End Source File
# Begin Source File

SOURCE=.\res\Royale_TreeExpandCollapse15.png
# End Source File
# End Group
# Begin Group "Ini Files"

# PROP Default_Filter "ini"
# Begin Source File

SOURCE=.\res\Royale_ExtraLarge.ini
# End Source File
# Begin Source File

SOURCE=.\res\Royale_LargeFonts.ini
# End Source File
# Begin Source File

SOURCE=.\res\Royale_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Royale_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Themes.ini
# End Source File
# End Group
# End Target
# End Project
