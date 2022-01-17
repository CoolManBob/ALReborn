#if !defined(AFX_LSTRANSLISTBOX_H__1DE15F21_6DE3_11D4_8CDF_0008C73F82B8__INCLUDED_)
#define AFX_LSTRANSLISTBOX_H__1DE15F21_6DE3_11D4_8CDF_0008C73F82B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTransparentListBox.h : header file
//

//#include "LSRgnBmpScrollButton.h"
#include <afxtempl.h>
#include <vector>		//. sub string array
#include "KbcButton.h"

/////////////////////////////////////////////////////////////////////////////
// CTransparentListBox window
typedef CMap<long,long,long,long> CMapWordToWord;

class CTransparentListBox : public CListBox
{
	// Construction
public:
	CTransparentListBox();

	// Attributes
public:

	// Operations
public:
	void SetFont(int nPointSize, CString FaceName,COLORREF Color,COLORREF SelColor,BOOL Shadow=TRUE,int SOffset = 2,COLORREF ShadowColor = RGB(0,0,0));
	void SetColor(COLORREF Color,COLORREF SelColor,COLORREF ShadowColor = RGB(0,0,0));

	BOOL SetTopIndex(int Index);
	int ScrollUp(int Lines=1);
	int ScrollDown(int Lines=1);
	int AddString(CString Text,DWORD ItemData = 0,BOOL Enabled = TRUE);
	void AddSubString(CString Text);
	int InsertString(int Index,CString Text, DWORD ItemData = 0,BOOL Enabled = TRUE);
	virtual void ResetContent(BOOL bInvalidate = TRUE);

	//. 2006. 4. 12. nonstopdj
	void SetItemBaseImage(INT ResourceID, COLORREF fontColor)
	{
		m_bmpItemBase.LoadBitmap(ResourceID);
		m_bmpItemBase.SetFontColor(fontColor);
	}

	void ClearAllListItem()
	{
		CListBox::ResetContent();
		m_arSubString.clear();
	}


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentListBox)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CTransparentListBox();

	// Generated message map functions
protected:
	int            m_ItemHeight;
	COLORREF       m_Color;
	COLORREF       m_SelColor;
	COLORREF       m_ShadowColor;
	CFont          m_Font;
	CBitmap        m_Bmp;
	BOOL           m_HasBackGround;
	int            m_ShadowOffset;
	BOOL           m_Shadow;
	long           m_PointSize;

	//. 2006. 4. 11. nonstopdj
	//. warning!! 1 x 4 slice button image.
	CKbcBmp			m_bmpItemBase;

	//. 2006. 4. 12. nonstopdj
	std::vector<CString>	m_arSubString;

	virtual CFont *GetFont();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual void DrawItem(CDC &Dc,int Index,CRect &Rect,BOOL Selected);


	//{{AFX_MSG(CTransparentListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnLbnSelchange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnTimer(UINT nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LSTRANSLISTBOX_H__1DE15F21_6DE3_11D4_8CDF_0008C73F82B8__INCLUDED_)

