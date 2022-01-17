#if !defined(AFX_RGBSELECTDLG_H__BC35E81A_FD18_46AB_85C3_BAAA7CB4D82D__INCLUDED_)
#define AFX_RGBSELECTDLG_H__BC35E81A_FD18_46AB_85C3_BAAA7CB4D82D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RgbSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRgbSelectDlg dialog
//#include "RgbSelectStatic.h"
#include "HSVControlStatic.h"

#include "RwCore.h"

class CRgbSelectDlg : public CDialog
{
// Construction
public:
	CRgbSelectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRgbSelectDlg)
	enum { IDD = IDD_LIGHTSETTINGDIALOG };
	CHSVControlStatic	m_ctlDirectionalRGB	;
	CHSVControlStatic	m_ctlAmbiantRGB		;
	//}}AFX_DATA

public:
	RwRGBAReal	m_AmbiantColor		;
	RwRGBAReal	m_DirectionalColor	;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRgbSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRgbSelectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBackcolorset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RGBSELECTDLG_H__BC35E81A_FD18_46AB_85C3_BAAA7CB4D82D__INCLUDED_)
