#if !defined(AFX_ADJUSTHEIGHTDLG_H__68B44E62_C226_49F4_AC6E_C72C2F983B34__INCLUDED_)
#define AFX_ADJUSTHEIGHTDLG_H__68B44E62_C226_49F4_AC6E_C72C2F983B34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustHeightDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdjustHeightDlg dialog

class CAdjustHeightDlg : public CDialog
{
// Construction
public:
	enum
	{
		INSERT_HEIGHT	,	// 높이값 대입.
		ADD_HEIGHT		,
		ADJUST_HEIGHT		// 비율로 곱하기.
	};
	CAdjustHeightDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdjustHeightDlg)
	enum { IDD = IDD_ADJUST_GEOMETRY_HEIGHT };
	CString	m_strValue;
	//}}AFX_DATA

	int		m_nType	;
	float	m_fValue;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjustHeightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdjustHeightDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUSTHEIGHTDLG_H__68B44E62_C226_49F4_AC6E_C72C2F983B34__INCLUDED_)
