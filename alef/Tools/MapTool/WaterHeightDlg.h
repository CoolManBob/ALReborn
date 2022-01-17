#if !defined(AFX_WATERHEIGHTDLG_H__0560AF7C_B0A4_4B1A_92BA_28EA97063298__INCLUDED_)
#define AFX_WATERHEIGHTDLG_H__0560AF7C_B0A4_4B1A_92BA_28EA97063298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaterHeightDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaterHeightDlg dialog

class CWaterHeightDlg : public CDialog
{
// Construction
public:
	CWaterHeightDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaterHeightDlg)
	enum { IDD = IDD_WATER_HEIGHT };
	float	m_fHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaterHeightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaterHeightDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERHEIGHTDLG_H__0560AF7C_B0A4_4B1A_92BA_28EA97063298__INCLUDED_)
