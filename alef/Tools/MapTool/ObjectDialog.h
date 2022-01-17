#if !defined(AFX_OBJECTDIALOG_H__1AFF60A5_C820_4311_8410_8A3511F01376__INCLUDED_)
#define AFX_OBJECTDIALOG_H__1AFF60A5_C820_4311_8410_8A3511F01376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog dialog

class CObjectDialog : public CDialog
{
// Construction
public:
	CObjectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectDialog)
	enum { IDD = IDD_DIALOG_OBJECT };
	CButton	m_ctlForcePreLight;
	CEdit	m_ctlAlpha;
	CEdit	m_ctlBlue;
	CEdit	m_ctlGreen;
	CEdit	m_ctlRed;
	CString	m_strName;
	CString	m_strFile;
	BYTE	m_nObjectRed;
	BYTE	m_nObjectGreen;
	BYTE	m_nObjectBlue;
	BOOL	m_bPreLight;
	BOOL	m_bForcePreLight;
	BOOL	m_bLight;
	BOOL	m_bMaterialColor;
	BOOL	m_bAmbient;
	BYTE	m_nObjectAlpha;
	BOOL	m_bAlpha;
	BOOL	m_bNoCameraAlpha;
	BOOL	m_bBlocking;
	BOOL	m_bRidable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectDialog)
	afx_msg void OnButtonBrowse();
	afx_msg void OnObjectPrelight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTDIALOG_H__1AFF60A5_C820_4311_8410_8A3511F01376__INCLUDED_)
