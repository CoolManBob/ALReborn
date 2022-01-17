#if !defined(AFX_MAPSELECTDLG_H__B1BB4C34_EDE3_46E3_9FF8_F2368BE8C90D__INCLUDED_)
#define AFX_MAPSELECTDLG_H__B1BB4C34_EDE3_46E3_9FF8_F2368BE8C90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapSelectDlg.h : header file
//

#include "MapSelectStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CMapSelectDlg dialog

class CMapSelectDlg : public CDialog
{
// Construction
public:
	CMapSelectDlg(CWnd* pParent = NULL);   // standard constructor

	int	m_nMode;

	int DoModal( int nMode );

// Dialog Data
	//{{AFX_DATA(CMapSelectDlg)
	enum { IDD = IDD_MAPSELECT };
	CComboBox	m_ctlLoadTypeCombo;
	CMapSelectStatic	m_MapSelectStatic;
	int		m_nX;
	int		m_nY;
	int		m_nX2;
	int		m_nY2;
	CString	m_strInfo;
	BOOL	m_bUseEffect;
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	DWORD	m_nType		;

	DWORD	m_nX_array	;
	DWORD	m_nY_array	;
	DWORD	m_nX2_array	;
	DWORD	m_nY2_array	;

	INT32	GetLoadedRegion()
	{
		// 로드됀 리전 체크
		return m_MapSelectStatic.GetMap();
	}

	BOOL	LoadSelection( CString strFilename );
	BOOL	SaveSelection( CString strFilename );

	BOOL	AddSelection( INT32 nDivision );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMapSelectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnExport();
	afx_msg void OnLoadall();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnTogglefield();
	afx_msg void OnExportmode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSaveselection();
	afx_msg void OnBnClickedLoadselection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPSELECTDLG_H__B1BB4C34_EDE3_46E3_9FF8_F2368BE8C90D__INCLUDED_)
