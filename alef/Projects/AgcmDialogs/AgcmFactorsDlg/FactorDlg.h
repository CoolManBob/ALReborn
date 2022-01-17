#if !defined(AFX_FACTORDLG_H__FE0CF6BA_AE93_43E4_8CBA_5A45941DCCC3__INCLUDED_)
#define AFX_FACTORDLG_H__FE0CF6BA_AE93_43E4_8CBA_5A45941DCCC3__INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FactorDlg.h : header file
//

#include "../EventButton.h"

#define __FACTOR_DLG_MAX_EVENT_BUTTON__	30

/////////////////////////////////////////////////////////////////////////////
// CFactorDlg dialog

class CFactorDlg : public CDialog
{
// Construction
public:
	CFactorDlg(AgpdFactor *pcsFactor, eAgpdFactorsType eFactorsType = AGPD_FACTORS_TYPE_NONE, INT32 lSubType = -1, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFactorDlg)
	enum { IDD = IDD_FACTOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

protected:
	AgpdFactor			*m_pcsFactor;
	eAgpdFactorsType	m_eAgpdFactorsType;
	INT32				m_lSubType;
	CEventButton		*m_paEventButton[__FACTOR_DLG_MAX_EVENT_BUTTON__];
	INT32				m_alTempSubType[__FACTOR_DLG_MAX_EVENT_BUTTON__];

public:
	AgpdFactor			*GetFactor()	{return m_pcsFactor;}

	VOID				SetTextEventButton(CHAR *szText, INT32 lIndex);
	VOID				GetTextEventButton(CHAR *pszText, INT32 lIndex);

protected:
	static BOOL PressEventButtonCB(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3);

	VOID ReleaseEventButton();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFactorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFactorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
*/
#endif // !defined(AFX_FACTORDLG_H__FE0CF6BA_AE93_43E4_8CBA_5A45941DCCC3__INCLUDED_)
