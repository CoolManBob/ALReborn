# Microsoft Developer Studio Project File - Name="Office2007Blue" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Office2007Blue - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Office2007Blue.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Office2007Blue.mak" CFG="Office2007Blue - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Office2007Blue - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
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
# ADD RSC /l 0x409 /i "../../../" /d "NDEBUG" /d "_AFXDLL" /d "_XTP_INCLUDE_VERSION"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /nodefaultlib /out:"../Office2007Blue.dll" /opt:nowin98 /noentry
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "Office2007Blue - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Office2007Blue.rc
# End Source File
# End Group
# Begin Group "Image Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\Office2007_ContextTabBlue.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabBlueClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabBlueGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabBlueHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabCyan.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabCyanClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabCyanGroupButtton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabCyanHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabGreen.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabGreenClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabGreenGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabGreenHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabGroupCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabGroupClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabOrange.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabOrangeClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabOrangeGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabOrangeHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabPurple.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabPurpleClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabPurpleGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabPurpleHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabRed.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabRedClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabRedGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabRedHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabSeparator.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabYellow.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabYellowClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabYellowGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ContextTabYellowHeader.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryPopup.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollArrowGlyphs.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollArrowsVerticalDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollArrowsVerticalLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollThumbGripperVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollThumbVertical.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollVerticalDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryScrollVerticalLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ControlGalleryUp.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameBottomLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameBottomRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionClose13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionClose17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionClose23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMaximize13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMaximize17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMaximize23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMinimize13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMinimize17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionMinimize23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionRestore13.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionRestore17.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameCaptionRestore23.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameSimpleTopRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameSystemButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameTopCenter.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameTopLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_FrameTopRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MenuCheckedItem.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MenuCheckedItemMark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MenuSelectedDisabledItem22.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MenuSelectedDisabledItem54.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MenuSelectedItem22.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_MenuSelectedItem54.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_PopupBarFrame.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupButtonIcon.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupCaption.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupClient.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupQuickAccess.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroups.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupsScrollLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupsScrollLeftGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupsScrollRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonGroupsScrollRightGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonOptionButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonQuickAccessButton.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonQuickAccessFrame.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonQuickAccessMore.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonTab.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_RibbonTabSeparator.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusBarDark.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusBarDevider.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusBarGripper.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusBarLight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_StatusBarSeparator.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonCheckBox.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonDropDownGlyph.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtons22.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtons50.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsComboDropDown.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialCenter.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSingle.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSplitCenter.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSplitDropDownCenter.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSplitDropDownLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSplitDropDownRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSplitLeft.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSpecialSplitRight.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplit22.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplit50.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplitDropDown22.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_ToolbarButtonsSplitDropDown50.png
# End Source File
# Begin Source File

SOURCE=.\res\Office2007_WorkspaceTopLeft.png
# End Source File
# End Group
# Begin Group "Ini Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\Office2007Theme.ini
# End Source File
# End Group
# End Target
# End Project
