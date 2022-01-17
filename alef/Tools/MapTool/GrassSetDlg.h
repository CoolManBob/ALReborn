#if !defined(AFX_GRASSSETDLG_H__EF59B38F_529F_4567_837B_CA8864A9F2A5__INCLUDED_)
#define AFX_GRASSSETDLG_H__EF59B38F_529F_4567_837B_CA8864A9F2A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrassSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrassSetDlg dialog

class CGrassSetDlg : public CDialog
{
// Construction
public:
	CGrassSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrassSetDlg)
	enum { IDD = IDD_GRASS_SET };
	BOOL	m_bGrassDraw;
	UINT	m_iDensity;
	float	m_fScaleMin;
	float	m_fScaleMax;
	int		m_iRotMin;
	int		m_iRotMax;
	int		m_iRotYMin;
	int		m_iRotYMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrassSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrassSetDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRASSSETDLG_H__EF59B38F_529F_4567_837B_CA8864A9F2A5__INCLUDED_)
