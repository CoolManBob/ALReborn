// XHeaderCtrl.h  Version 1.4
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XHEADERCTRL_H
#define XHEADERCTRL_H

#define DT_DEFAULT	((UINT)-1)		//+++

#define FLATHEADER_TEXT_MAX	80

#define XHEADERCTRL_NO_IMAGE		0
#define XHEADERCTRL_UNCHECKED_IMAGE	1
#define XHEADERCTRL_CHECKED_IMAGE	2

///////////////////////////////////////////////////////////////////////////////
// CXHeaderCtrl window

#define FH_PROPERTY_SPACING			1
#define FH_PROPERTY_ARROW			2
#define FH_PROPERTY_STATICBORDER	3
#define FH_PROPERTY_DONTDROPCURSOR	4
#define FH_PROPERTY_DROPTARGET		5


class CXHeaderCtrl : public CHeaderCtrl
{
    DECLARE_DYNCREATE(CXHeaderCtrl)

// Construction
public:
	CXHeaderCtrl();
	virtual ~CXHeaderCtrl();

// Attributes
public:
	void SetListCtrl(CListCtrl *pListCtrl) { m_pListCtrl = pListCtrl; }
	BOOL ModifyProperty(WPARAM wParam, LPARAM lParam);
	int GetSpacing() { return m_iSpacing; }
	void SetSpacing(int nSpacing) { m_iSpacing = nSpacing; }
	UINT GetAlignment() { return m_nFormat; }						//+++
	void SetAlignment(UINT nFormat) { m_nFormat = nFormat; }		//+++
	COLORREF GetTextColor() { return m_rgbText; }					//+++
	void SetTextColor(COLORREF rgbText) { m_rgbText = rgbText; }	//+++
	BOOL GetDividerLines() { return m_bDividerLines; }				//+++
	void EnableDividerLines(BOOL bEnable) { m_bDividerLines = bEnable; }	//+++

// Overrides
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void DrawItem(CDC* pDC, CRect rect, LPHDITEM lphdi);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListCtrl * m_pListCtrl;	//+++
	UINT m_nFormat;				//+++
	COLORREF m_rgbText;			//+++
	BOOL m_bDividerLines;		//+++
	BOOL m_bDoubleBuffer;
	BOOL m_bStaticBorder;
	BOOL m_bResizing;
	int m_iSpacing;
	SIZE m_sizeImage;
	SIZE m_sizeArrow;
	UINT m_nDontDropCursor;
	UINT m_nClickFlags;
	CPoint m_ptClickPoint;

	COLORREF m_cr3DHighLight;
	COLORREF m_cr3DShadow;
	COLORREF m_cr3DFace;
	COLORREF m_crBtnText;

	void DrawCtrl(CDC* pDC);
	int DrawImage(CDC* pDC, CRect rect, LPHDITEM hdi, BOOL bRight);
	int DrawBitmap(CDC* pDC, CRect rect, LPHDITEM hdi, CBitmap* pBitmap, 
		BITMAP* pBitmapInfo, BOOL bRight);
	int DrawText (CDC* pDC, CRect rect, LPHDITEM lphdi);

// Generated message map functions
protected:
	//{{AFX_MSG(CXHeaderCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnDeleteItem(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnInsertItem(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnLayout(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnSetImageList(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XHEADERCTRL_H
