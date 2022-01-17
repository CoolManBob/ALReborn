# Microsoft Developer Studio Project File - Name="WinXP.Luna" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WinXP.Luna - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinXP.Luna.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinXP.Luna.mak" CFG="WinXP.Luna - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinXP.Luna - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP BASE Output_Dir "Release/vc50"
# PROP BASE Intermediate_Dir "Release/vc50"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release/vc50"
# PROP Intermediate_Dir "Release/vc50"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "XTPRESOURCEARSA_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../../../Source" /I "../../../Source/Common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "XTPRESOURCE_EXPORTS" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /nodefaultlib /out:"../WinXP.Luna.cjstyles" /opt:nowin98 /noentry
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "WinXP.Luna - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\WinXP.Luna.rc
# End Source File
# End Group
# Begin Group "Image Files"

# PROP Default_Filter "bmp"
# Begin Source File

SOURCE=.\res\Blue_Button.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CheckBox13.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CheckBox16.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CheckBox25.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_Chevron.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CloseGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CloseGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CloseGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_CloseGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ComboButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ComboButtonGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ExplorerBarHeaderClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ExplorerBarHeaderPin.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FieldOutlineBlue.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameBottom.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameCaptionMin.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameMaximized.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_FrameRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_Gripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_GripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_GroupBox.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_HelpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_HelpGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_HelpGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_HelpGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_HelpGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ListViewHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ListviewHeaderBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MaximizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MaximizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MaximizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MaximizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MaximizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MDICaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MDIGlyphClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MDIGlyphMinimize.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MDIGlyphRestore.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MinimizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MinimizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MinimizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MinimizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_MinimizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_NormalGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_NormalGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_NormalGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_NormalGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_PlaceBarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_PlaceBarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ProgressChunk.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ProgressChunkVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ProgressTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ProgressTrackVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RadioButton13.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RadioButton16.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RadioButton25.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ResizeGrip2.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RestoreGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RestoreGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RestoreGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RestoreGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_RestoreGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollArrowGlyphs.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollArrowGlyphsSmall.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollArrows.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollShaftHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollShaftVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollThumbGripperHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollThumbGripperVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollThumbHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ScrollThumbVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_Separator.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SeparatorVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SliderTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SmallCloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SmallCloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SmallFrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SmallFrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpecialGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpecialGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpecialGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpecialGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinButtonBackgroundDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinButtonBackgroundLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinButtonBackgroundRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinButtonBackgroundUp.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinLeftGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinRightGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_SpinUpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_StatusBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_StatusPane.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabBackground133.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItem.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemTop.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemTopBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemTopLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabItemTopRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TabPaneEdge.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarButtonsSplit.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarButtonsSplitDropdown.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarButtonsSplitDropdownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_ToolbarGripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarDown13.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarDown16.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarDown25.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackbarHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarLeft13.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarLeft16.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarLeft25.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackbarRight13.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarRight16.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarRight25.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarUp13.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarUp16.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackBarUp25.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TrackbarVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TreeExpandCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TreeExpandCollapse10.png
# End Source File
# Begin Source File

SOURCE=.\res\Blue_TreeExpandCollapse15.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_button.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CheckBox13.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CheckBox16.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CheckBox25.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_Chevron.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CloseGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CloseGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CloseGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_CloseGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ComboButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ComboButtonGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ExplorerBarHeaderClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ExplorerBarHeaderPin.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_FieldOutlineBlue.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_frameBottom.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_FrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_FrameCaptionMin.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_FrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_frameLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_FrameMaximized.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_frameRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_Gripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_GripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_GroupBox.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_HelpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_HelpGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_HelpGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_HelpGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_HelpGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ListViewHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ListviewHeaderBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MaximizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MaximizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MaximizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MaximizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MaximizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MDICaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MDIGlyphClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MDIGlyphMinimize.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MDIGlyphRestore.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MinimizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MinimizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MinimizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MinimizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_MinimizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_NormalGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_NormalGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_NormalGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_NormalGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_PlaceBarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_PlaceBarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ProgressChunk.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ProgressChunkVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ProgressTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ProgressTrackVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RadioButton13.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RadioButton16.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RadioButton25.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ResizeGrip2.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RestoreGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RestoreGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RestoreGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RestoreGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_RestoreGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollArrowGlyphs.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollArrowGlyphsSmall.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollArrows.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollShaftHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollShaftVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollThumbGripperHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollThumbGripperVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollThumbHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ScrollThumbVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_Separator.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SeparatorVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_sliderTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SmallCloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SmallCloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SmallFrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SmallFrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpecialGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpecialGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpecialGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpecialGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinButtonBackgroundDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinButtonBackgroundLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinButtonBackgroundRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinButtonBackgroundUp.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinLeftGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinRightGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_SpinUpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_StatusBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_StatusPane.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TabBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TabBackground133.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItem.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemTop.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemTopBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemTopLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_tabItemTopRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TabPaneEdge.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarButtonsSplit.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarButtonsSplitDropdown.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarButtonsSplitDropdownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ToolbarGripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarDown13.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarDown16.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarDown25.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackbarHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarLeft13.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarLeft16.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarLeft25.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackbarRight13.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarRight16.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarRight25.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarUp13.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarUp16.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackBarUp25.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_TrackbarVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_treeExpandCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_treeExpandCollapse10.png
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_treeExpandCollapse15.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_button.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CheckBox13.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CheckBox16.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CheckBox25.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_Chevron.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CloseGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CloseGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CloseGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_CloseGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ComboButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ComboButtonGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ExplorerBarHeaderClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ExplorerBarHeaderPin.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_FieldOutlineBlue.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_frameBottom.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_FrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_FrameCaptionMin.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_FrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_frameLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_FrameMaximized.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_frameRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_Gripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_GripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_GroupBox.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_HelpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_HelpGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_HelpGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_HelpGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_HelpGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ListViewHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ListviewHeaderBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MaximizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MaximizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MaximizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MaximizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MaximizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MDICaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MDIGlyphClose.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MDIGlyphMinimize.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MDIGlyphRestore.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MinimizeGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MinimizeGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MinimizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MinimizeGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_MinimizeGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_NormalGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_NormalGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_NormalGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_NormalGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_PlaceBarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_PlaceBarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ProgressChunk.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ProgressChunkVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ProgressTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ProgressTrackVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RadioButton13.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RadioButton16.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RadioButton25.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ResizeGrip2.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RestoreGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RestoreGlyph19.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RestoreGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RestoreGlyph6.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_RestoreGlyph9.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollArrowGlyphs.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollArrowGlyphsSmall.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollArrows.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollShaftHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollShaftVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollThumbGripperHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollThumbGripperVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollThumbHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ScrollThumbVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_Separator.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SeparatorVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_sliderTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SmallCloseButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SmallCloseGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SmallFrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SmallFrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpecialGroupBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpecialGroupCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpecialGroupExpand.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpecialGroupHead.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinButtonBackgroundDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinButtonBackgroundLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinButtonBackgroundRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinButtonBackgroundUp.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinLeftGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinRightGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_SpinUpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_StatusBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_StatusPane.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TabBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TabBackground133.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItem.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemTop.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemTopBoth.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemTopLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_tabItemTopRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TabPaneEdge.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarButtonsSplit.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarButtonsSplitDropdown.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarButtonsSplitDropdownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ToolbarGripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarDown13.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarDown16.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarDown25.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackbarHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarLeft13.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarLeft16.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarLeft25.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackbarRight13.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarRight16.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarRight25.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarUp13.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarUp16.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackBarUp25.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_TrackbarVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_treeExpandCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_treeExpandCollapse10.png
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_treeExpandCollapse15.png
# End Source File
# End Group
# Begin Group "Ini Files"

# PROP Default_Filter "ini"
# Begin Source File

SOURCE=.\res\Blue_ExtraLarge.ini
# End Source File
# Begin Source File

SOURCE=.\res\Blue_LargeFonts.ini
# End Source File
# Begin Source File

SOURCE=.\res\Blue_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Blue_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_ExtraLarge.ini
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_LargeFonts.ini
# End Source File
# Begin Source File

SOURCE=.\res\Homestead_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_ExtraLarge.ini
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_LargeFonts.ini
# End Source File
# Begin Source File

SOURCE=.\res\Metallic_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Themes.ini
# End Source File
# End Group
# End Target
# End Project
