// XTPShadowsManager.h : interface for the CXTPShadowsManager class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#if !defined(__XTPSHADOWSMANAGER_H__)
#define __XTPSHADOWSMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPHookManager.h"


//#define NOALPHASHADOW

class CXTPControlPopup;
class CXTPCommandBar;


//===========================================================================
// Summary:
//     CXTPShadowsManager is standalone class used to manage CommandBars' shadows.
//===========================================================================
class _XTP_EXT_CLASS CXTPShadowsManager
{
private:
	typedef BOOL(WINAPI* LPFNUPDATELAYEREDWINDOW)
		(
		HWND hwnd,             // Handle to layered window
		HDC hdcDst,            // Handle to screen DC
		POINT *pptDst,         // New screen position
		SIZE *psize,           // New size of the layered window
		HDC hdcSrc,            // Handle to surface DC
		POINT *pptSrc,         // Layer position
		COLORREF crKey,        // Color key
		BLENDFUNCTION *pblend, // Blend function
		DWORD dwFlags          // Options
		);

	class CShadowList;

private:
	class CShadowWnd : public CWnd, public CXTPHookManagerHookAble
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CShadowWnd object.
		//-------------------------------------------------------------------------
		CShadowWnd();

	public:
		BOOL  Create(BOOL bHoriz, CRect rcWindow);
		void LongShadow(CShadowList* pList);
		BOOL ExcludeRect(CRect rcExclude);

	private:
		DECLARE_MESSAGE_MAP()

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnPaint();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg LRESULT OnNcHitTest(CPoint point);

	private:
		virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
		UINT Factor(int& nRed, int& nGreen, int& nBlue, double dFactor);
		BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	private:

		BOOL m_bHoriz;
		int m_nShadowOptions;
		CXTPCommandBar* m_pCommandBar;

		friend class CXTPShadowsManager;
	};


	//===========================================================================
	// Summary:
	//     Shadow list
	//===========================================================================
	class _XTP_EXT_CLASS CShadowList : public CList<CShadowWnd*, CShadowWnd*>
	{
	public:

		void AddShadow(CShadowWnd* pShadow)
		{
			pShadow->LongShadow(this);
			AddTail(pShadow);
		}

		void RemoveShadow(CShadowWnd* pShadow)
		{
			POSITION pos = Find(pShadow);
			ASSERT(pos);
			RemoveAt(pos);
		}
	};

private:
	//===========================================================================
	// Summary:
	//     Constructs a CXTPShadowsManager object.
	//===========================================================================
	CXTPShadowsManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPShadowsManager object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPShadowsManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Check the system alpha shadow ability.
	// Returns:
	//     TRUE if alpha shadow available; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL AlphaShadow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the command bar  shadow.
	// Parameters:
	//     pCommandBar - Points to a CXTPCommandBar object
	//     rcExclude - Excluded rectangle.
	//     pControl - Points to a CXTPControlPopup object
	//-----------------------------------------------------------------------
	void SetShadow(CXTPControlPopup* pControl);
	void SetShadow(CXTPCommandBar* pCommandBar, const CRect& rcExclude = CRect(0, 0, 0, 0)); // <combine CXTPShadowsManager::SetShadow@CXTPControlPopup*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes shadows for the command bar.
	// Parameters:
	//     pCommandBar - Points to a CXTPCommandBar object
	//-----------------------------------------------------------------------
	void RemoveShadow(CXTPCommandBar* pCommandBar);

private:
	void DestroyShadow(CShadowWnd*);
	CShadowWnd* CreateShadow(BOOL bHoriz, CRect rc, CRect rcExclude, CXTPCommandBar* pCommandBar);


private:
	LPFNUPDATELAYEREDWINDOW m_pfnUpdateLayeredWindow;
	CShadowList m_lstShadow;
	BOOL m_bAlphaShadow;

	friend class CShadowWnd;
	friend _XTP_EXT_CLASS CXTPShadowsManager* AFX_CDECL XTPShadowsManager();
};

_XTP_EXT_CLASS CXTPShadowsManager* AFX_CDECL XTPShadowsManager();


AFX_INLINE BOOL CXTPShadowsManager::CShadowWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

#endif //#if !defined(__XTPSHADOWSMANAGER_H__)
