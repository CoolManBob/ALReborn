#if !defined(AFX_TILENAMEEDITDLG_H__BEFACCAB_8B5D_48BE_8911_C131EA1386E6__INCLUDED_)
#define AFX_TILENAMEEDITDLG_H__BEFACCAB_8B5D_48BE_8911_C131EA1386E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TileNameEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTileNameEditDlg dialog

class CTileNameEditDlg : public CDialog
{
// Construction
public:
	CTileNameEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTileNameEditDlg)
	enum { IDD = IDD_TILE_NAME_EDIT };
	CString	m_strName;
	int		m_nNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTileNameEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTileNameEditDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TILENAMEEDITDLG_H__BEFACCAB_8B5D_48BE_8911_C131EA1386E6__INCLUDED_)
