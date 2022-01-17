#if !defined(AFX_OBJECTBRUSHSETTINGDLG_H__DB6942E1_69E5_46D4_B090_CE2BAD07C0A9__INCLUDED_)
#define AFX_OBJECTBRUSHSETTINGDLG_H__DB6942E1_69E5_46D4_B090_CE2BAD07C0A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectBrushSettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectBrushSettingDlg dialog

class CObjectBrushSettingDlg : public CDialog
{
// Construction
public:
	CObjectBrushSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectBrushSettingDlg)
	enum { IDD = IDD_OBJECT_BRUSH_SETTING };
	float	m_fDensity;
	float	m_fRotateMax;
	float	m_fRotateMin;
	float	m_fScaleMax;
	float	m_fScaleMin;
	float	m_fRotateYMin;
	float	m_fRotateYMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectBrushSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectBrushSettingDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTBRUSHSETTINGDLG_H__DB6942E1_69E5_46D4_B090_CE2BAD07C0A9__INCLUDED_)
