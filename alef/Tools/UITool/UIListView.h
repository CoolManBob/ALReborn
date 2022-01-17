#if !defined(AFX_UILISTVIEW_H__D29CFD71_5438_4CC7_B664_E40987387B15__INCLUDED_)
#define AFX_UILISTVIEW_H__D29CFD71_5438_4CC7_B664_E40987387B15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UIListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUIListView view

class CUIListView : public CListView
{
public:
	CUIListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CUIListView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUIListView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUIListView();

	BOOL SortTextItems( int nCol, BOOL bAscending,int low = 0, int high = -1 );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CUIListView)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UILISTVIEW_H__D29CFD71_5438_4CC7_B664_E40987387B15__INCLUDED_)
