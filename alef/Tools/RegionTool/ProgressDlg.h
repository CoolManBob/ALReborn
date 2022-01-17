#if !defined(AFX_PROGRESSDLG_H__E85EE98B_0932_4531_A4BC_1FB02D8322DE__INCLUDED_)
#define AFX_PROGRESSDLG_H__E85EE98B_0932_4531_A4BC_1FB02D8322DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
public:
	static	int	m_nRefCount;
	// 사용은 이 함수로!.
	BOOL	StartProgress	( CString message , int target , CWnd * pParent );
	int		SetProgress		( int progress );
	int		SetTarget		( int progress );
	void	EndProgress		();
	BOOL	SetMessage		( CString str );

// Construction
public:
	void Render( CDC * pDC = NULL );
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	~CProgressDlg();

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS_DLG };
	CString	m_strPercent;
	CString	m_strMessage;
	//}}AFX_DATA
	int		m_nPopupLevel;
	COLORREF	m_rgbBackground;
	CString		m_strMaptoolMessage;

	int		m_nTarget	;
	int		m_nCurrent	;

	CDC		m_memDC		;
	CBitmap	m_bmpDC		;
	CRect	m_rectProgress;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__E85EE98B_0932_4531_A4BC_1FB02D8322DE__INCLUDED_)
