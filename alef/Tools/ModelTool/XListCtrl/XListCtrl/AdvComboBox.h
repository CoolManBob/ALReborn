/////////////////////////////////////////////////////////////////////////////
// AdvComboBox.h : header file
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

#ifndef ADVCOMBOBOX_H
#define ADVCOMBOBOX_H

#include "DropListBox.h"
#include "DropWnd.h"
#include "AdvComboEdit.h"
#include <tchar.h>
typedef std::basic_string<TCHAR> tstring;

#define WM_ON_DROPDOWN_BUTTON  WM_USER+101

#define ACBS_FLAT			0x0001
#define ACBS_STANDARD		0x0002
#define ACBS_CHECKED		0x0004
#define ACBS_AUTOAPPEND		0x0008
#define ACBS_AUTOSUGGEST	0x0010

#pragma warning( disable : 4251 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4275 )

#include <list>
#include <map>
#include <string>
using namespace std;

void AFXAPI DDX_ACBIndex( CDataExchange* pDX, int nIDC, int& index );
void AFXAPI DDX_ACBString( CDataExchange* pDX, int nIDC, CString& value );


/////////////////////////////////////////////////////////////////////////////
// CAdvComboBox window
//#define ADVCOMBOBOXCTRL_CLASSNAME    _T("AdvComboBoxCtrl")  // Window class name
#define ADVCOMBOBOXCTRL_CLASSNAME    _T("SuperComboBox")  // Window class name

class CAdvComboBox : public CWnd
{
// Construction
public:
	CAdvComboBox( BOOL bInst = FALSE, CWnd * pParent = NULL );

	DECLARE_DYNAMIC(CAdvComboBox)

// Attributes
public:
	CWnd * GetDropWnd() { return m_pDropWnd; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvComboBox)
public:
	virtual BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void OnEscapeKey() {};		//+++
	virtual void OnComboComplete() {};		//+++

// Implementation
public:
	void LoadString( UINT nStringID = 0 );
	int GetDefaultVisibleItems();
	void SetDefaultVisibleItems( int nItems = -1 );
	virtual CFont* GetFont() {return m_pFont;}
	int GetMinVisibleItems();
	void SetMinVisibleItems( int nMinItems );
	BOOL LimitText( int nMaxChars );
	BOOL SetEditSel( int nStartChar, int nEndChar );
	DWORD GetEditSel();
	int FindStringExact( int nIndexStart, LPCTSTR lpszFind );
	int InsertString( int nIndex, LPCTSTR lpszString );
	int DeleteString( UINT nIndex );
	BOOL GetExtendedUI();
	int SetExtendedUI( BOOL bExtended = TRUE );
	BOOL GetDroppedState();
	void GetDroppedControlRect( LPRECT lprect );
	void ShowDropDown( BOOL bShowIt = TRUE );
	int InitStorage( int nItems, UINT nBytes );
	int SetTopIndex( int nIndex );
	int GetTopIndex();
	void SetItemChecked( int nIndex, BOOL bChecked );
	BOOL GetItemChecked( int nIndex );
	void SetItemDisabled( int nIndex, BOOL bDisabled );
	BOOL GetItemDisabled( int nIndex );
	void ResetContent();
	void* GetItemDataPtr( int nIndex );
	int SetItemDataPtr( int nIndex, void* pData );
	DWORD GetItemData( int nIndex );
	int SetItemData( int nIndex, DWORD dwItemData );
	int SetCurSel( int nSelect );
	int GetCount();
	int GetCurSel();
	void SetText( LPCTSTR lpszText );
	void GetText( CString& rString );
	int GetText( LPTSTR lpszText );
	int AddString( LPCTSTR lpszString );
	int FindString( int nStartAfter, LPCTSTR lpszString );
	int SelectString( int nStartAfter, LPCTSTR lpszString );
	void GetLBText( int nIndex, CString& rString );
	int GetLBText( int nIndex, LPTSTR lpszText );
	int GetLBTextLen(int nIndex );
	int SetItemHeight( int nIndex,  int nHeight );
	virtual ~CAdvComboBox();

	CRect& GetComboRect() { return m_rcCombo; }
	void ModifyACBStyle( UINT nRemoveStyle, UINT nAddStyle );
	BOOL PointInWindow( CPoint ptScreenPoint );

	UINT GetComboWidth();	//+++

	// Generated message map functions
protected:
	//{{AFX_MSG(CAdvComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnChildActivate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	//}}AFX_MSG
	afx_msg void OnMouseLeave();
	afx_msg LONG OnSelectedItem( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnDropdownButton( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnDestroyDropdownList( WPARAM wParam, LPARAM lParam );
	afx_msg void OnKillfocusEdit();
	afx_msg void OnSetfocusEdit();
	afx_msg void OnChangeEdit();
	afx_msg void OnUpdateEdit();

	// ComboBox messages
	afx_msg LONG OnAddString( WPARAM wParam, LPARAM lString );
	afx_msg LONG OnSetCurSel( WPARAM wIndex, LPARAM lParam );
	afx_msg LONG OnGetCurSel( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnSelectString( WPARAM wItemStart, LPARAM lString );
	afx_msg LONG OnGetCount( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnGetLBText( WPARAM wIndex, LPARAM lString );
	afx_msg LONG OnGetLBTextLen( WPARAM wIndex, LPARAM lParam );
	afx_msg LONG OnGetTopIndex( WPARAM wParam, LPARAM lParam );
	afx_msg LONG OnSetTopIndex( WPARAM wIndex, LPARAM lParam );

	DECLARE_MESSAGE_MAP()

    BOOL RegisterWindowClass();

private:
	void CreateDropList( list<LIST_ITEM> &droplist );
	void SelPrevItem();
	void SelNextItem();
	void SelPrevPage();
	void SelNextPage();

	BOOL m_bInst;
	CWnd * m_pParent;
	int m_nCurSel;
	CAdvComboEdit* m_pEdit;	//+++
	tstring m_strEdit;
	CFont* m_pFont;
	DWORD m_dwStyle;
	DWORD m_dwACBStyle;

	CRect m_rcCombo;
	CRect m_rcStaticWnd;
	CRect m_rcDropWnd;


	CDropWnd* m_pDropWnd;
	CRect m_rcDropButton;
	BOOL m_bDropListVisible;
	bool m_bDropRectStored;

	list<LIST_ITEM> m_list;
	list<LIST_ITEM>::iterator m_iter;

	int m_zDelta; // MouseWheel...

	bool m_bHasFocus;
	bool m_bHasSentFocus;
	bool m_bSelItem;
	bool m_bFirstPaint;
	int m_nMinVisItems;
	bool m_bCodeCreate;
	BOOL m_bAutoAppend;
	bool m_bDropButtonHot;
	bool m_bTrackMouseLeave;

	int m_nDefaultDropItems; // Number of items to be shown when dropwindow first shown.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //ADVCOMBOBOX_H
