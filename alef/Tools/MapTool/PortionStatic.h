#if !defined(AFX_PORTIONSTATIC_H__4AE47753_FCE2_4E08_89C5_9749209B49B9__INCLUDED_)
#define AFX_PORTIONSTATIC_H__4AE47753_FCE2_4E08_89C5_9749209B49B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PortionStatic.h : header file
//

// 마고자 (2003-11-21 오후 3:34:45) : Event Nature에 있는거 가져다가 고쳐서 씀..
// 부레에에에엑;;

/////////////////////////////////////////////////////////////////////////////
// CPortionStatic window
#define	PORTIONSTATIC_MAX_ELEMENT	10
#define	WM_PORTIONL_NOTIFY	( WM_USER + 4864 )

class CPortionStatic : public CStatic
{
// Construction
public:
	CPortionStatic();

	struct	Element
	{
		INT32	nRate	;
		CString	strName	;	// 이름.
	};
protected:

	// 저장됀것~
	Element	m_aElement[ PORTIONSTATIC_MAX_ELEMENT ];
	INT32	m_nElement;

	INT32	GetPortionTotal();
	INT32	GetTemporaryTotal();
	INT32	m_nSelectedElement;

// Attributes
public:
	BOOL	AddElement( CString strName , INT32 nRate );
	INT32	GetCount(){ return m_nElement; }
	Element *	GetElement( INT32 nIndex );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPortionStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPortionStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPortionStatic)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PORTIONSTATIC_H__4AE47753_FCE2_4E08_89C5_9749209B49B9__INCLUDED_)
