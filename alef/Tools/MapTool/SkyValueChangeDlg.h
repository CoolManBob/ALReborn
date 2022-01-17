#if !defined(AFX_SKYVALUECHANGEDLG_H__402067EC_79B2_497D_8F61_15BF572678E6__INCLUDED_)
#define AFX_SKYVALUECHANGEDLG_H__402067EC_79B2_497D_8F61_15BF572678E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkyValueChangeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkyValueChangeDlg dialog

class CSkyValueChangeDlg : public CDialog
{
// Construction
public:
	CSkyValueChangeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSkyValueChangeDlg)
	enum { IDD = IDD_SKY_VALUE_CHANGE };
	float	m_fCircumstanceHeight;
	float	m_fCircumstanceRadius;
	float	m_fSkyHeight;
	float	m_fSkyRadius;
	int		m_nCircumstanceHeight;
	int		m_nCircumstanceRadius;
	int		m_nSkyHeight;
	int		m_nSkyRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkyValueChangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSkyValueChangeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKYVALUECHANGEDLG_H__402067EC_79B2_497D_8F61_15BF572678E6__INCLUDED_)
