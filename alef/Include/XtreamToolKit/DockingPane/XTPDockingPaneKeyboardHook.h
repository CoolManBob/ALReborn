// XTPDockingPaneKeyboardHook.h : interface for the CXTPDockingPaneKeyboardHook class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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
#if !defined(__XTPDOCKINGPANEKEYBOARDHOOK_H__)
#define __XTPDOCKINGPANEKEYBOARDHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXTPDockingPane;
class CXTPDockingPaneManager;

//===========================================================================
// Summary:
//     CXTPDockingPaneWindowSelect is a CMiniFrameWnd derived class used to
//     show window selector for Ctrl+F9 and Alt+F7 handlers
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneWindowSelect : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CXTPDockingPaneWindowSelect)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Item type enumeration
	//-----------------------------------------------------------------------
	enum ItemType
	{
		itemPane,       // Item is docking pane
		itemMDIFrame,   // Item is handle of MDI child
		itemSDIFrame    // Item is handle of SDI frame
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Item description
	//-----------------------------------------------------------------------
	struct CItem
	{
		CRect rc;       // Item bounding rectangle

		union
		{
			CXTPDockingPane* pPane;
			HWND hWndFrame;
		};

		int nIndex;     // Index of the item
		int nColumn;    // Column of the index
		int nRow;       // Row of the item
		ItemType type;  // Item type
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Column description
	//-----------------------------------------------------------------------
	struct CColumn
	{
		int nFirst;
		int nLast;
	};


protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneWindowSelect object.
	//-----------------------------------------------------------------------
	CXTPDockingPaneWindowSelect();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneMiniWnd object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPaneWindowSelect();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to hit test item in specified position
	// Parameters:
	//     pt - Client coordinates to test
	//-----------------------------------------------------------------------
	CItem* HitTest(CPoint pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to select specified item
	// Parameters:
	//     pItem - Item to select
	//-----------------------------------------------------------------------
	void Select(CItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to select item by its index
	// Parameters:
	//     nItem - Index of item to select
	//-----------------------------------------------------------------------
	void Select(int nItem);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get item caption
	// Parameters:
	//     pItem - Pointer to item which caption need to retrieve
	//-----------------------------------------------------------------------
	virtual CString GetItemCaption(CItem* pItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get item description
	// Parameters:
	//     pItem - Pointer to item which description need to retrieve
	//-----------------------------------------------------------------------
	virtual CString GetItemDescription(CItem* pItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get item path string
	// Parameters:
	//     pItem - Pointer to item which path string need to retrieve
	//-----------------------------------------------------------------------
	virtual CString GetItemPath(CItem* pItem) const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to update position of all items inside dialog.
	// Returns:
	//     Returns TRUE is successful;
	//-----------------------------------------------------------------------
	virtual BOOL Reposition();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPDockingPaneWindowSelect)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPDockingPaneWindowSelect)
	public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	int CalcItemHeight(CDC* pDC);
	BOOL DoModal();
	HWND GetMDIClient() const;

private:
	CXTPDockingPaneManager* m_pManager;      // Owner DockingPaneMaanager class
	CArray<CItem*, CItem*> m_arrItems;       // Items in Windows Selector
	CArray<CColumn, CColumn&> m_arrColumns;  // Columns collection
	CItem* m_pSelected;                      // Selected item
	HCURSOR m_hHandCursor;                   // Hand cursor handle
	HCURSOR m_hArrowCursor;                  // Arrow cursor handle
	int m_nFirstFile;                        // Index of first file in Items
	int m_nPaneCount;                        // Total number of panes

	CString m_strActiveTools;                // ActiveTools string
	CString m_strActiveFiles;                // ActiveFiles string

	CFont m_fnt;                             // Normal font
	CFont m_fntBold;                         // Bold font
	BOOL m_bActivatePanes;                   // TRUE to activate panes

	friend class CXTPDockingPaneKeyboardHook;
};

//{{AFX_CODEJOCK_PRIVATE

#include "XTPDockingPaneManager.h"


class _XTP_EXT_CLASS CXTPDockingPaneKeyboardHook : public CNoTrackObject
{
public:
	CXTPDockingPaneKeyboardHook();
	~CXTPDockingPaneKeyboardHook();

public:
	void SetupKeyboardHook(CXTPDockingPaneManager* pManager, BOOL bSetup);
	CXTPDockingPaneManager* FindFocusedManager();

	static CXTPDockingPaneKeyboardHook* AFX_CDECL GetThreadState();

protected:
	static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);

	CXTPDockingPaneManager* Lookup(HWND hSite) const;

protected:
	HHOOK m_hHookKeyboard;          // Keyboard hook

	static CThreadLocal<CXTPDockingPaneKeyboardHook> _xtpKeyboardThreadState;           // Instance of Keyboard hook

	CMap<HWND, HWND, CXTPDockingPaneManager*, CXTPDockingPaneManager*> m_mapSites;
#ifdef _AFXDLL
	AFX_MODULE_STATE* m_pModuleState; // Module state
#endif

	CXTPDockingPaneWindowSelect* m_pWindowSelect;
};


AFX_INLINE CXTPDockingPaneKeyboardHook* AFX_CDECL CXTPDockingPaneKeyboardHook::GetThreadState() {
	return _xtpKeyboardThreadState.GetData();
}



//}}AFX_CODEJOCK_PRIVATE

#endif //#if !defined(__XTPDOCKINGPANEKEYBOARDHOOK_H__)
