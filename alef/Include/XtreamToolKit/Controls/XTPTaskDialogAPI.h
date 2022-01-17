// XTPTaskDialogAPI.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPTASKAPI_H__)
#define __XTPTASKAPI_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE
namespace XTPTaskDialogAPI
{
	#ifndef PBST_NORMAL
	#define PBST_NORMAL 0x0001
	#endif

	#ifndef PBST_ERROR
	#define PBST_ERROR  0x0002
	#endif

	#ifndef PBST_PAUSED
	#define PBST_PAUSED 0x0003
	#endif

	#ifndef IS_INTRESOURCE
	#define IS_INTRESOURCE(x) (((size_t)(x) >> 16) == 0)
	#endif

// Check if TaskDialog already included in comctrl.h.
#ifndef TD_WARNING_ICON

#pragma pack(push,1)

	typedef HRESULT (CALLBACK *PFTASKDIALOGCALLBACK)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData);

	enum _TASKDIALOG_FLAGS
	{
		TDF_ENABLE_HYPERLINKS               = 0x0001,
		TDF_USE_HICON_MAIN                  = 0x0002,
		TDF_USE_HICON_FOOTER                = 0x0004,
		TDF_ALLOW_DIALOG_CANCELLATION       = 0x0008,
		TDF_USE_COMMAND_LINKS               = 0x0010,
		TDF_USE_COMMAND_LINKS_NO_ICON       = 0x0020,
		TDF_EXPAND_FOOTER_AREA              = 0x0040,
		TDF_EXPANDED_BY_DEFAULT             = 0x0080,
		TDF_VERIFICATION_FLAG_CHECKED       = 0x0100,
		TDF_SHOW_PROGRESS_BAR               = 0x0200,
		TDF_SHOW_MARQUEE_PROGRESS_BAR       = 0x0400,
		TDF_CALLBACK_TIMER                  = 0x0800,
		TDF_POSITION_RELATIVE_TO_WINDOW     = 0x1000,
		TDF_RTL_LAYOUT                      = 0x2000,
		TDF_NO_DEFAULT_RADIO_BUTTON         = 0x4000,
		TDF_CAN_BE_MINIMIZED                = 0x8000
	};
	typedef int TASKDIALOG_FLAGS;                         // Note: _TASKDIALOG_FLAGS is an int

	typedef enum _TASKDIALOG_MESSAGES
	{
		TDM_NAVIGATE_PAGE                   = WM_USER+101,
		TDM_CLICK_BUTTON                    = WM_USER+102, // wParam = Button ID
		TDM_SET_MARQUEE_PROGRESS_BAR        = WM_USER+103, // wParam = 0 (nonMarque) wParam != 0 (Marquee)
		TDM_SET_PROGRESS_BAR_STATE          = WM_USER+104, // wParam = new progress state
		TDM_SET_PROGRESS_BAR_RANGE          = WM_USER+105, // lParam = MAKELPARAM(nMinRange, nMaxRange)
		TDM_SET_PROGRESS_BAR_POS            = WM_USER+106, // wParam = new position
		TDM_SET_PROGRESS_BAR_MARQUEE        = WM_USER+107, // wParam = 0 (stop marquee), wParam != 0 (start marquee), lparam = speed (milliseconds between repaints)
		TDM_SET_ELEMENT_TEXT                = WM_USER+108, // wParam = element (TASKDIALOG_ELEMENTS), lParam = new element text (LPCWSTR)
		TDM_CLICK_RADIO_BUTTON              = WM_USER+110, // wParam = Radio Button ID
		TDM_ENABLE_BUTTON                   = WM_USER+111, // lParam = 0 (disable), lParam != 0 (enable), wParam = Button ID
		TDM_ENABLE_RADIO_BUTTON             = WM_USER+112, // lParam = 0 (disable), lParam != 0 (enable), wParam = Radio Button ID
		TDM_CLICK_VERIFICATION              = WM_USER+113, // wParam = 0 (unchecked), 1 (checked), lParam = 1 (set key focus)
		TDM_UPDATE_ELEMENT_TEXT             = WM_USER+114, // wParam = element (TASKDIALOG_ELEMENTS), lParam = new element text (LPCWSTR)
		TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE = WM_USER+115, // wParam = Button ID, lParam = 0 (elevation not required), lParam != 0 (elevation required)
		TDM_UPDATE_ICON                     = WM_USER+116  // wParam = icon element (TASKDIALOG_ICON_ELEMENTS), lParam = new icon (hIcon if TDF_USE_HICON_* was set, PCWSTR otherwise)
	} TASKDIALOG_MESSAGES;

	typedef enum _TASKDIALOG_NOTIFICATIONS
	{
		TDN_CREATED                         = 0,
		TDN_NAVIGATED                       = 1,
		TDN_BUTTON_CLICKED                  = 2,            // wParam = Button ID
		TDN_HYPERLINK_CLICKED               = 3,            // lParam = (LPCWSTR)pszHREF
		TDN_TIMER                           = 4,            // wParam = Milliseconds since dialog created or timer reset
		TDN_DESTROYED                       = 5,
		TDN_RADIO_BUTTON_CLICKED            = 6,            // wParam = Radio Button ID
		TDN_DIALOG_CONSTRUCTED              = 7,
		TDN_VERIFICATION_CLICKED            = 8,             // wParam = 1 if checkbox checked, 0 if not, lParam is unused and always 0
		TDN_HELP                            = 9,
		TDN_EXPANDO_BUTTON_CLICKED          = 10            // wParam = 0 (dialog is now collapsed), wParam != 0 (dialog is now expanded)
	} TASKDIALOG_NOTIFICATIONS;

	typedef struct _TASKDIALOG_BUTTON
	{
		int     nButtonID;
		PCWSTR  pszButtonText;
	} TASKDIALOG_BUTTON;

	typedef enum _TASKDIALOG_ELEMENTS
	{
		TDE_CONTENT,
		TDE_EXPANDED_INFORMATION,
		TDE_FOOTER,
		TDE_MAIN_INSTRUCTION,
	} TASKDIALOG_ELEMENTS;

	typedef enum _TASKDIALOG_ICON_ELEMENTS
	{
		TDIE_ICON_MAIN,
		TDIE_ICON_FOOTER
	} TASKDIALOG_ICON_ELEMENTS;

	#define TD_WARNING_ICON         MAKEINTRESOURCEW(-1)
	#define TD_ERROR_ICON           MAKEINTRESOURCEW(-2)
	#define TD_INFORMATION_ICON     MAKEINTRESOURCEW(-3)
	#define TD_SHIELD_ICON          MAKEINTRESOURCEW(-4)

	enum _TASKDIALOG_COMMON_BUTTON_FLAGS
	{
		TDCBF_OK_BUTTON            = 0x0001, // selected control return value IDOK
		TDCBF_YES_BUTTON           = 0x0002, // selected control return value IDYES
		TDCBF_NO_BUTTON            = 0x0004, // selected control return value IDNO
		TDCBF_CANCEL_BUTTON        = 0x0008, // selected control return value IDCANCEL
		TDCBF_RETRY_BUTTON         = 0x0010, // selected control return value IDRETRY
		TDCBF_CLOSE_BUTTON         = 0x0020  // selected control return value IDCLOSE
	};
	typedef int TASKDIALOG_COMMON_BUTTON_FLAGS;           // Note: _TASKDIALOG_COMMON_BUTTON_FLAGS is an int

	typedef struct _TASKDIALOGCONFIG
	{
		UINT        cbSize;
		HWND        hwndParent;
		HINSTANCE   hInstance;                              // used for MAKEINTRESOURCE() strings
		TASKDIALOG_FLAGS                dwFlags;            // TASKDIALOG_FLAGS (TDF_XXX) flags
		TASKDIALOG_COMMON_BUTTON_FLAGS  dwCommonButtons;    // TASKDIALOG_COMMON_BUTTON (TDCBF_XXX) flags
		PCWSTR      pszWindowTitle;                         // string or MAKEINTRESOURCE()
		union
		{
			HICON   hMainIcon;
			PCWSTR  pszMainIcon;
		};
		PCWSTR      pszMainInstruction;
		PCWSTR      pszContent;
		UINT        cButtons;
		const TASKDIALOG_BUTTON  *pButtons;
		int         nDefaultButton;
		UINT        cRadioButtons;
		const TASKDIALOG_BUTTON  *pRadioButtons;
		int         nDefaultRadioButton;
		PCWSTR      pszVerificationText;
		PCWSTR      pszExpandedInformation;
		PCWSTR      pszExpandedControlText;
		PCWSTR      pszCollapsedControlText;
		union
		{
			HICON   hFooterIcon;
			PCWSTR  pszFooterIcon;
		};
		PCWSTR      pszFooter;
		PFTASKDIALOGCALLBACK pfCallback;
		LONG_PTR    lpCallbackData;
		UINT        cxWidth;                                // width of the Task Dialog's client area in DLU's. If 0, Task Dialog will calculate the ideal width.
	} TASKDIALOGCONFIG;

#pragma pack(pop)

#endif

//}}AFX_CODEJOCK_PRIVATE

	_XTP_EXT_CLASS int AFXAPI XTPLoadStringInst(HINSTANCE hInstance, UINT nID, CString* pString);

	_XTP_EXT_CLASS CPoint AFXAPI XTPDlu2Pix(int dluX, int dluY);

	_XTP_EXT_CLASS CPoint AFXAPI XTPPix2Dlu(int pixX, int pixY);

	_XTP_EXT_CLASS BOOL AFXAPI XTPCalcTextSize(const CString& strText, CSize& pSizeText, CFont& font, BOOL* pbWordWrap = NULL);

	_XTP_EXT_CLASS BOOL AFXAPI XTPCalcIdealTextSize(const CString& strText, CSize& pSizeText, CFont& font, int dluMax = 330, int dluGrow = 7);

	_XTP_EXT_CLASS CString AFXAPI XTPExtractSubString(CString strFullString, int iSubString);

	_XTP_EXT_CLASS CRect AFXAPI XTPSize2Rect(CSize size);
}

using namespace XTPTaskDialogAPI;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPTASKAPI_H__)
