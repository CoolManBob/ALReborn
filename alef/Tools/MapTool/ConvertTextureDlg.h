#if !defined(AFX_CONVERTTEXTUREDLG_H__C8DFAE2A_B011_438D_8B25_B46EED2B96A2__INCLUDED_)
#define AFX_CONVERTTEXTUREDLG_H__C8DFAE2A_B011_438D_8B25_B46EED2B96A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvertTextureDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConvertTextureDlg dialog

// 타일 내용을 화면에 표시해주고..
// 타일 타입을 입력 받음.
#include "bmp.h"

class CConvertTextureDlg : public CDialog
{
// Construction
public:
	CConvertTextureDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConvertTextureDlg)
	enum { IDD = IDD_CONVERT_TILE_DIALOG };
	CComboBox	m_ctlProperty;
	CComboBox	m_ctlComboDimension;
	CComboBox	m_ctlTypeCombo;
	CComboBox	m_ctlCategoryCombo	;
	CString		m_strName			;
	CString		m_strFileName		;
	BOOL		m_bShowGrid			;
	//}}AFX_DATA

	CString		m_strFileOriginalPath;
	CString		m_strFileNameOrigin	;
	CBmp		m_bmpImage			;
	CRect		m_rectImage			;

	BOOL		m_bDisableType		;
	BOOL		m_bTextureUpdate	;
	// 결과물..
	int			m_nType				;
	int			m_nDimension		;
	int			m_nProperty			;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConvertTextureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConvertTextureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeType();
	afx_msg void OnPaint();
	afx_msg void OnCloseupDimension();
	afx_msg void OnShowgrid();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERTTEXTUREDLG_H__C8DFAE2A_B011_438D_8B25_B46EED2B96A2__INCLUDED_)
