#if !defined(AFX_INPUTDLG_H__6D3B8726_8B1E_4AB8_AF0A_94C08667C863__INCLUDED_)
#define AFX_INPUTDLG_H__6D3B8726_8B1E_4AB8_AF0A_94C08667C863__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog

class CInputDlg : public CDialog
{
// Construction
public:
	CInputDlg(INT32 *plDestValue, CWnd* pParent = NULL);   // standard constructor

protected:
	INT32	*m_plDestValue;

// Dialog Data
	//{{AFX_DATA(CInputDlg)
	enum { IDD = IDD_INPUT };
	int		m_nInput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDLG_H__6D3B8726_8B1E_4AB8_AF0A_94C08667C863__INCLUDED_)
