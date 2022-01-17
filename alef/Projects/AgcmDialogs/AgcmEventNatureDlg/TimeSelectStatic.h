#if !defined(AFX_TIMESELECTSTATIC_H__C96E1B32_6B6E_449D_8FDD_683C7361C7C7__INCLUDED_)
#define AFX_TIMESELECTSTATIC_H__C96E1B32_6B6E_449D_8FDD_683C7361C7C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeSelectStatic.h : header file
//

#include "AgcmEventNature.h"
#define	WM_TIMESELECT_EVENT	( WM_USER + 13513 )

#define	TSS_TIME_HEIGHT		20

// Time 셀렉트.. 
/////////////////////////////////////////////////////////////////////////////
// CTimeSelectStatic window

class CTimeSelectStatic : public CStatic
{
// Construction
public:
	CTimeSelectStatic();

// Attributes
public:

// Data
protected:
	
	BOOL	m_abTime[ ASkySetting::TS_MAX ]	;
	BOOL	m_bLButtonDown	;
	INT32	m_nLButtonTime	;

// Operations
public:
	INT32	SetTime( INT32	time	);	// 시간을 지정함..
			// 설정하면서 화면을 업데이트함..
	INT32	AdditionalSetTime( INT32	time , BOOL bSelect = TRUE	);	// 셀렉션 추가..
	
	INT32	GetSelectCount();

	INT32	GetTime( INT32 nOffset = 0 );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeSelectStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimeSelectStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeSelectStatic)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMESELECTSTATIC_H__C96E1B32_6B6E_449D_8FDD_683C7361C7C7__INCLUDED_)
