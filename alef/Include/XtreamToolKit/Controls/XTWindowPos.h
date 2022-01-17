// XTWindowPos.h interface for the CXTWindowPos class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#if !defined(__XTWINDOWPLACEMENT_H__)
#define __XTWINDOWPLACEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
// Summary:
//     CXTWindowPos is a WINDOWPLACEMENT structure derived class. It extends
//     the WINDOWPLACEMENT structure, and is used to save and restore window
//     position.
//===========================================================================
class _XTP_EXT_CLASS CXTWindowPos : public WINDOWPLACEMENT
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTWindowPos object
	//-----------------------------------------------------------------------
	CXTWindowPos();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will load the window specified by 'pWnd' to its
	//     previous window position.
	// Parameters:
	//     pWnd       - Points to the CWnd* derived window to be restored to its previous state.
	//     lpszWndPos - If NULL, the default entry name will be used. If using this for MDI
	//                  children or other windows, pass in a unique string value here. This
	//                  must match for both LoadWindowPos and SaveWindowPos.
	//     lpszSection - A NULL terminated string representing the registry section where the
	//                   window position is to be stored. If NULL the default value is used.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL LoadWindowPos(LPCTSTR lpszWndPos = NULL, LPCTSTR lpszSection = NULL);
	BOOL LoadWindowPos(CWnd* pWnd, LPCTSTR lpszWndPos = NULL, LPCTSTR lpszSection = NULL); // <combine CXTWindowPos::LoadWindowPos@LPCTSTR@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will save the window specified by 'pWnd' by its
	//     current window position.
	// Parameters:
	//     pWnd       - Points to the CWnd* derived windows position to be saved.
	//     lpszWndPos - If NULL, the default entry name will be used. If using this for MDI
	//                  children or other windows, pass in a unique string value here. This
	//                  must match for both LoadWindowPos and SaveWindowPos.
	//     lpszSection - A NULL terminated string representing the registry section where the
	//                   window position is to be stored. If NULL the default value is used.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SaveWindowPos(CWnd* pWnd, LPCTSTR lpszWndPos = NULL, LPCTSTR lpszSection = NULL);
};

#endif // #if !defined(__XTWINDOWPLACEMENT_H__)
