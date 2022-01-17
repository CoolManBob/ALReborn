#if !defined(AFX_ATOMICFILTERDLG_H__A13FE99A_D8F8_4F0B_983D_6C986C0C4E3D__INCLUDED_)
#define AFX_ATOMICFILTERDLG_H__A13FE99A_D8F8_4F0B_983D_6C986C0C4E3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AtomicFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAtomicFilterDlg dialog

class CAtomicFilterDlg : public CDialog
{
// Construction
public:
	CAtomicFilterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAtomicFilterDlg)
	enum { IDD = IDD_ATOMICFILTER };
	BOOL	m_bGeometry			;
	BOOL	m_bSystemObject		;
	BOOL	m_bObjectAll		;
	BOOL	m_bRidableObject	;
	BOOL	m_bBlockingObject	;
	BOOL	m_bEventFilter		;
	BOOL	m_bGrass			;
	BOOL	m_bObjectOthers		;
	CString	m_strEventCombo;
	//}}AFX_DATA

	INT32	m_nSelectedEventID	;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtomicFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAtomicFilterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnClickButton(UINT);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATOMICFILTERDLG_H__A13FE99A_D8F8_4F0B_983D_6C986C0C4E3D__INCLUDED_)
