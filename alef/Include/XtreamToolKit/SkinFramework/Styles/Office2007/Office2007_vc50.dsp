# Microsoft Developer Studio Project File - Name="Office2007" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Office2007 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Office2007.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Office2007.mak" CFG="Office2007 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Office2007 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
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
# ADD LINK32 /nologo /dll /machine:I386 /nodefaultlib /out:"../Office2007.cjstyles" /opt:nowin98 /noentry
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "Office2007 - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Office2007.rc
# End Source File
# End Group
# Begin Group "Image Files"

# PROP Default_Filter "bmp"
# Begin Source File

SOURCE=.\res\Office2007_Button.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_CaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_CheckBox13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_CloseGlyph13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_CloseGlyph17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_CloseGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ComboButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ComboButtonGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameBottom.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMin.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionSizing.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameMaximized.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_Gripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_GripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_GroupBox.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_HelpGlyph13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_HelpGlyph17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ListViewHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ListViewHeaderBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MaximizeGlyph13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MaximizeGlyph17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MaximizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MinimizeGlyph13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MinimizeGlyph17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MinimizeGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ProgressChunk.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ProgressTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RadioButton13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ResizeGrip2.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RestoreGlyph13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RestoreGlyph17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RestoreGlyph23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollArrowGlyphs.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollArrows.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollArrowsHorizontalDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollArrowsHorizontalLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollArrowsVerticalDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollArrowsVerticalLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollHorizontalDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollHorizontalLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollThumbGripperHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollThumbGripperVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollThumbHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollThumbVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollVerticalDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ScrollVerticalLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_Separator.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SeparatorVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SliderTrack.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SmallFrameCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinButtonBackgroundDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinButtonBackgroundLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinButtonBackgroundRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinButtonBackgroundUp.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinLeftGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinRightGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_SpinUpGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusPane.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TabBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TabItemTop.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TabItemTopLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TabPaneEdge.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarBackground.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtons.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplit.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplitDropDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplitDropDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarGripperVert.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TrackbarDown13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TrackbarHorizontal.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TrackbarLeft13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TrackbarRight13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TrackbarUp13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TrackbarVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TreeExpandCollapse.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TreeExpandCollapse10.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_TreeExpandCollapse15.png
# End Source File
# End Group
# Begin Group "Ini Files"

# PROP Default_Filter "ini"
# Begin Source File

SOURCE=.\res\Office2007_Normal.ini
# End Source File
# Begin Source File

SOURCE=.\res\Themes.ini
# End Source File
# End Group
# End Target
# End Project
