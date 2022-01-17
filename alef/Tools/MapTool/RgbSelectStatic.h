#if !defined(AFX_RGBSELECTSTATIC_H__46398C2B_0E7A_43D3_8C24_D2B9612C81DF__INCLUDED_)
#define AFX_RGBSELECTSTATIC_H__46398C2B_0E7A_43D3_8C24_D2B9612C81DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RgbSelectStatic.h : header file
//

// RGB 칼라를 선택하기 위한 스태틱 윈도우.
#define	WM_RGBSELECTUPDATE	( WM_USER + 1010 )
// 변경시에 페어런트로 넘어감.
#define	RGBSELECTCONTROL_WIDTH	200
#define	RGBSELECTCONTROL_HEIGHT	89
#define	RGBSELECT_MARGIN		2
#define	RGBSELECT_COLOR_WIDTH	145
#define	RGBSELECT_BRIGHT_WIDTH	17
#define	RGBSELECT_PREVIEW		30

/////////////////////////////////////////////////////////////////////////////
// CRgbSelectStatic window

class CRgbSelectStatic : public CStatic
{
// Construction
public:
	CRgbSelectStatic();

	BOOL		m_bLButtonDown;

	CDC			m_memDC;

	COLORREF	m_RGB;	// RGB칼라.

	COLORREF	m_PickedColor;

	int			m_nRGBX;
	int			m_nRGBY;
	int			m_nBrightY;

	CBitmap		m_Bitmap;

	void	SetRGB( COLORREF rgb );

// Attributes
public:

// Operations
public:
	void	Init( COLORREF pickedcolor );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRgbSelectStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Update();
	virtual ~CRgbSelectStatic();

	// Generated message map functions
public:
	//{{AFX_MSG(CRgbSelectStatic)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RGBSELECTSTATIC_H__46398C2B_0E7A_43D3_8C24_D2B9612C81DF__INCLUDED_)
