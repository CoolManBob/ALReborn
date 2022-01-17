// XTPSyntaxEditDefines.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITDEFINES_H__)
#define __XTPSYNTAXEDITDEFINES_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Edit action flags
#define XTP_EDIT_EDITACTION_INSERTROW            0x01 // Insert row
#define XTP_EDIT_EDITACTION_DELETEROW            0x02 // Delete row
#define XTP_EDIT_EDITACTION_MODIFYROW            0x04 // Modify row
//#define XTP_EDIT_EDITACTION_SELECTIONCHANGED     0x08 // Selection changed
#define XTP_EDIT_EDITACTION_INSERTTEXT           0x100 // Generic insert text action
#define XTP_EDIT_EDITACTION_DELETETEXT           0x200 // Generic delete text action

//////////////////////////////////
// Notification codes
//////////////////////////////////

// First and last entries
#define XTP_EDIT_NM_FIRST                   (0U - 951U)         // First notification ID for SyntaxEdit
#define XTP_EDIT_NM_LAST                    (0U - 979U)         // Last notification ID for SyntaxEdit

// Following are the codes for various notifications
#define XTP_EDIT_NM_EDITCHANGING               XTP_EDIT_NM_FIRST + 0  // Edit changing will be called before changing content
#define XTP_EDIT_NM_EDITCHANGED                XTP_EDIT_NM_FIRST + 1  // This will be called if content has been changed
#define XTP_EDIT_NM_SETDOCMODIFIED             XTP_EDIT_NM_FIRST + 2  // Indicates if the document is modified
#define XTP_EDIT_NM_DRAWBOOKMARK               XTP_EDIT_NM_FIRST + 3  // This will be called while drawing the bookmarks
#define XTP_EDIT_NM_ROWCOLCHANGED              XTP_EDIT_NM_FIRST + 4  // This will be fired if row or col has been changed
#define XTP_EDIT_NM_INSERTKEY                  XTP_EDIT_NM_FIRST + 8  // Update all of the vertical panes
#define XTP_EDIT_NM_SELINIT                    XTP_EDIT_NM_FIRST + 9  // Updates all the selections while one pane initiates one
#define XTP_EDIT_NM_STARTOLEDRAG               XTP_EDIT_NM_FIRST + 10 // Start OLE drag
#define XTP_EDIT_NM_MARGINCLICKED              XTP_EDIT_NM_FIRST + 11 // Left mouse button was clicked in the gutter area
#define XTP_EDIT_NM_UPDATESCROLLPOS            XTP_EDIT_NM_FIRST + 12 // Edit control scroll positions changed.
#define XTP_EDIT_NM_PARSEEVENT                 XTP_EDIT_NM_FIRST + 13 // This notification used to reflect parser events. See XTPSyntaxEditOnParseEvent enum.

#define XTP_EDIT_FINDWORD_PREV              0x01 // Find start of the previous word
#define XTP_EDIT_FINDWORD_NEXT              0x02 // Find start of the next word

//{{AFX_CODEJOCK_PRIVATE
// send with XTP_EDIT_NM_UPDATESCROLLPOS but unused on higher layer.
#define XTP_EDIT_UPDATE_HORZ       0x01
#define XTP_EDIT_UPDATE_VERT       0x02
#define XTP_EDIT_UPDATE_DIAG       0x04
#define XTP_EDIT_UPDATE_ALL        (XTP_EDIT_UPDATE_HORZ | XTP_EDIT_UPDATE_VERT | XTP_EDIT_UPDATE_DIAG)
//}}AFX_CODEJOCK_PRIVATE

#define XTP_EDIT_AVELINELEN    30   // Average length of line. Used as default value for CXTPSyntaxEditCtrl::m_nAverageLineLen.

//{{AFX_CODEJOCK_PRIVATE
// default values for some parcer parameters
#define XTP_EDIT_LEXPARSER_MAXBACKOFFSETDEFAULT            100
#define XTP_EDIT_LEXPARSER_REPARSETIMEOUTMS                500
#define XTP_EDIT_LEXPARSER_ONSCREENSCHCACHELIFETIMESEC     180
#define XTP_EDIT_LEXPARSER_THREADIDLELIFETIMESEC           60
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
// Registry constants.
const TCHAR XTP_EDIT_REG_SETTINGS[]                    = _T("Settings");
const TCHAR XTP_EDIT_REG_AUTORELOAD[]                  = _T("AutoReload");
const TCHAR XTP_EDIT_REG_LINENUMBACKCOLOR[]            = _T("ColorLineNumberBack");
const TCHAR XTP_EDIT_REG_LINENUMTEXTCOLOR[]            = _T("ColorLineNumberText");
const TCHAR XTP_EDIT_REG_INSELBACKCOLOR[]              = _T("ColorInactiveHiliteColorBack");
const TCHAR XTP_EDIT_REG_INSELTEXTCOLOR[]              = _T("ColorInactiveHiliteColorText");
const TCHAR XTP_EDIT_REG_SELBACKCOLOR[]                = _T("ColorHiliteBack");
const TCHAR XTP_EDIT_REG_SELTEXTCOLOR[]                = _T("ColorHiliteText");
const TCHAR XTP_EDIT_REG_BACKCOLOR[]                   = _T("ColorBack");
const TCHAR XTP_EDIT_REG_TEXTCOLOR[]                   = _T("ColorText");
const TCHAR XTP_EDIT_REG_LOGFONT[]                     = _T("LogFont");
const TCHAR XTP_EDIT_REG_VSCROLLBAR[]                  = _T("VScrollBar");
const TCHAR XTP_EDIT_REG_HSCROLLBAR[]                  = _T("HScrollBar");
const TCHAR XTP_EDIT_REG_SYNTAXCOLOR[]                 = _T("SyntaxColor");
const TCHAR XTP_EDIT_REG_AUTOINDENT[]                  = _T("AutoIndent");
const TCHAR XTP_EDIT_REG_SELMARGIN[]                   = _T("SelMargin");
const TCHAR XTP_EDIT_REG_LINENUMBERS[]                 = _T("LineNumbers");
const TCHAR XTP_EDIT_REG_WIDECARET[]                   = _T("WideCaret");
const TCHAR XTP_EDIT_REG_TABWITHSPACE[]                = _T("TabWithSpace");
const TCHAR XTP_EDIT_REG_TABSIZE[]                     = _T("TabSize");
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
// Lexer classes' defines
const TCHAR XTP_EDIT_LEXCLASS_STARTTOKEN[]             = _T("lexClass:");
const TCHAR XTP_EDIT_LEXCLASS_ENDTOKEN[]               = _T("//:lexClass");
const TCHAR XTP_EDIT_LEXCLASS_PROPNAME[]               = _T("name");
const TCHAR XTP_EDIT_LEXCLASS_EOL[]                    = _T("\r\n");
const TCHAR XTP_EDIT_LEXCLASS_EMPTYSTR[]               = _T("");

const TCHAR XTP_EDIT_LEXPARSER_DEFTHEME[]              = _T("Default");
const TCHAR XTP_EDIT_LEXPARSER_CFG_FILENAME[]          = _T("SyntaxEdit.ini");
const TCHAR XTP_EDIT_LEXPARSER_PARENT_SCHEMA[]         = _T("parentschema");
const TCHAR XTP_EDIT_LEXPARSER_SECTION_MAIN[]          = _T("main");
const TCHAR XTP_EDIT_LEXPARSER_SECTION_THEMES[]        = _T("themes");
const TCHAR XTP_EDIT_LEXPARSER_SECTION_SCHEMES[]       = _T("schemes");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_COLORBK[]         = _T("colorBK");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_COLORFG[]         = _T("colorFG");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_COLORSELBK[]      = _T("colorSelBK");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_COLORSELFG[]      = _T("colorSelFG");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_FONTBOLD[]        = _T("Bold");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_FONTITALIC[]      = _T("Italic");
const TCHAR XTP_EDIT_LEXPARSER_PARAM_FONTUNDERLINE[]   = _T("Underline");

const TCHAR XTP_EDIT_CFMSDEVCOLSEL[]                   = _T("MSDEVColumnSelect");
const TCHAR XTP_EDIT_CLASSNAME_EDITCTRL[]              = _T("CodejockSyntaxEditor");
const TCHAR XTP_EDIT_CLASSNAME_AUTOCOMPLETEWND[]       = _T("CodejockSEAutoCompleteWnd");
const TCHAR XTP_EDIT_CLASSNAME_TOOLTIP[]               = _T("CodejockSEToolTip");
const TCHAR XTP_EDIT_CLASSNAME_LBOXTIP[]               = _T("CodejockSEListBoxTipWnd");
//}}AFX_CODEJOCK_PRIVATE

#define XTP_EDIT_RGB_GETRED(rgb)           (((rgb) >> 16) & 0xff)   // Get red color value for raw RGB color like "0xAABBCC".
#define XTP_EDIT_RGB_GETGREEN(rgb)         (((rgb) >> 8) & 0xff)    // Get green color value for raw RGB color like "0xAABBCC".
#define XTP_EDIT_RGB_GETBLUE(rgb)          ((rgb) & 0xff)           // Get blue color value for raw RGB color like "0xAABBCC".
#define XTP_EDIT_RGB_INT2CLR(rgb)          RGB(XTP_EDIT_RGB_GETRED(rgb),XTP_EDIT_RGB_GETGREEN(rgb),XTP_EDIT_RGB_GETBLUE(rgb)) // Convert raw RGB color value like "0xAABBCC" to COLORREF.

//===========================================================================
// Summary: Line endings types
//===========================================================================
enum XTPSyntaxEditCRLFStyle
{
	xtpEditCRLFStyleUnknown     = -1,       // Used to indicate unknown or error value
	xtpEditCRLFStyleDos         =  0,       // DOS style CRLF (0x0d0a)
	xtpEditCRLFStyleUnix        =  1,       // UNIX style CRLF (0x0a0d)
	xtpEditCRLFStyleMac         =  2        // MAC style CRLF (0x0a)
};

//===========================================================================
// Summary: Edit delete position
//===========================================================================
enum XTPSyntaxEditDeletePos
{
	xtpEditDelPosAfter,         // Delete char(s) after specified position.
	xtpEditDelPosBefore         // Delete char(s) before specified position.
};

//===========================================================================
// Summary: these events are sent from different threads. Its receiver must be thread safety.
//===========================================================================
enum XTPSyntaxEditOnParseEvent
{
	xtpEditOnParserStarted      = 1, // dwParam1 - <unused>
	xtpEditOnTextBlockParsed    = 2, // dwParam1 = Text Block ID
	xtpEditOnParserEnded        = 3  // dwParam1 = <status flags> : xtpEditLPR_RunFinished, xtpEditLPR_RunBreaked, xtpEditLPR_Error;
};

//===========================================================================
// Summary: these events are sent from different threads. Its receiver must be thread safety.
//===========================================================================
enum XTPSyntaxEditOnCfgChangedEvent
{
	xtpEditClassSchWasChanged   = 10, // dwParam1 = LPCTSTR - Schema name;  dwParam2 = CXTPSyntaxEditLexTextSchema* New text schema pointer, or NULL if it was removed.
	xtpEditThemeWasChanged      = 11, // dwParam1 = LPCTSTR - Theme name;   dwParam2 = CXTPSyntaxEditColorTheme* New theme pointer, or NULL if it was removed.
	xtpEditAllConfigWasChanged  = 12  // dwParam1 - <unused>, dwParam2 - <unused>
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITDEFINES_H__)
