#if !defined(AFX_HSVINPUTDLG_H__A4BD8C8D_D02A_4074_A139_B2EB13AB2A6A__INCLUDED_)
#define AFX_HSVINPUTDLG_H__A4BD8C8D_D02A_4074_A139_B2EB13AB2A6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HSVInputDlg.h : header file
//

#include "HSVControlStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CHSVInputDlg dialog

class CHSVInputDlg : public CDialog
{
// Construction
public:
	CHSVInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHSVInputDlg)
	enum { IDD = IDD_HSVINPUT };
	CHSVControlStatic	m_ctlHSVControl;
	//}}AFX_DATA

	int	m_nR;
	int	m_nG;
	int	m_nB;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHSVInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	UINT	DoModal( int nR , int nG , int nB );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHSVInputDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
// Construction
public:
	CColorStatic();

// Attributes
public:
	int		m_nR;	// 0~255
	int		m_nG;	// 0~255
	int		m_nB;	// 0~255

	COLORREF	GetRGB()
	{
		return RGB( m_nR , m_nG , m_nB );
	}
	void		SetRGB( int nR , int nG , int nB )
	{
		m_nR	= nR;
		m_nG	= nG;
		m_nB	= nB;

		Invalidate( TRUE );
	}
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSVINPUTDLG_H__A4BD8C8D_D02A_4074_A139_B2EB13AB2A6A__INCLUDED_)
