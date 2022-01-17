/////////////////////////////////////////////////////////////////////////////
// DropWnd.h : header file
// 
// CAdvComboBox Control
// Version: 2.1
// Date: September 2002
// Author: Mathias Tunared
// Email: Mathias@inorbit.com
// Copyright (c) 2002. All Rights Reserved.
//
// This code, in compiled form or as source code, may be redistributed 
// unmodified PROVIDING it is not sold for profit without the authors 
// written consent, and providing that this notice and the authors name 
// and all copyright notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef DROPWND_H
#define DROPWND_H

#pragma warning(push, 3)
#include <list>
#include <string>
#pragma warning(pop)
#include <tchar.h>
typedef std::basic_string<TCHAR> tstring;

using namespace std;
typedef struct _LIST_ITEM
{
	tstring strText;
	BOOL bDisabled;
	BOOL bChecked;
	void* vpItemData;

	_LIST_ITEM()
	{
		strText = _T("");
		bDisabled = FALSE;
		bChecked = FALSE;
		vpItemData = NULL;
	}

	BOOL operator <(_LIST_ITEM other)
	{
		if( strText < other.strText )
			return TRUE;
		else
			return FALSE;
	}
} LIST_ITEM, *PLIST_ITEM;

#include "DropListBox.h"
#include "DropScrollBar.h"

/////////////////////////////////////////////////////////////////////////////
// CDropWnd window

class CDropWnd : public CWnd
{
// Construction
public:
	CDropWnd( CWnd* pComboParent, list<LIST_ITEM> &itemlist, DWORD dwACBStyle );

// Attributes
public:
	CDropListBox*	GetListBoxPtr() { return m_listbox; }
	CDropScrollBar* GetScrollBarPtr() { return m_scrollbar; }

// Operations
public:
	list<LIST_ITEM>& GetList() { return m_list; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDropWnd)
public:
	//virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
	//	UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL DestroyWindow();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDropWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDropWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	afx_msg LONG OnSetCapture( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnReleaseCapture( WPARAM wParam, LPARAM lParam );

	//+++
#if (_MSC_VER > 1200)
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif

	DECLARE_MESSAGE_MAP()

private:
	int m_nMaxX;	//+++
	int m_nMaxY;	//+++
	CDropListBox* m_listbox;
	CRect m_rcList;
	CDropScrollBar* m_scrollbar;
	CRect m_rcScroll;
	CRect m_rcSizeHandle;

	CWnd* m_pComboParent;
	CFont* m_pListFont;

	list<LIST_ITEM> m_list;
	list<LIST_ITEM>::iterator m_iter;

	bool m_bResizing;
	CPoint m_ptLastResize;
	int m_nMouseDiffX;
	int m_nMouseDiffY;
	DWORD m_dwACBStyle;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //DROPWND_H
