#include "afxwin.h"
#if !defined(AFX_EXPORTDLG_H__007EF49B_60D2_4B99_B64C_02D7A412EC48__INCLUDED_)
#define AFX_EXPORTDLG_H__007EF49B_60D2_4B99_B64C_02D7A412EC48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

class CExportDlg : public CDialog
{
// Construction
public:
	CExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportDlg)
	enum { IDD = IDD_EXPORT };
	BOOL	m_bCompactData;
	BOOL	m_bMapDetail;
	BOOL	m_bObject;
	BOOL	m_bMapRough;
	BOOL	m_bTile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// 미니밉 저장용
	BOOL m_bMinimapExport;
	BOOL m_bServerDataExport;
	INT32 m_nCameraType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__007EF49B_60D2_4B99_B64C_02D7A412EC48__INCLUDED_)
