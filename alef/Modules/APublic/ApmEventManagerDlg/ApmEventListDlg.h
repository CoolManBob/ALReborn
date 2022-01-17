#if !defined(AFX_APMEVENTLISTDLG_H__3B9C39E7_F7B8_45C8_9E6B_ADA21B07085E__INCLUDED_)
#define AFX_APMEVENTLISTDLG_H__3B9C39E7_F7B8_45C8_9E6B_ADA21B07085E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApmEventListDlg.h : header file
//

#include "resource.h"
#include "ApmEventManager.h"

/////////////////////////////////////////////////////////////////////////////
// ApmEventListDlg dialog

class ApmEventListDlg : public CDialog
{
private:
	ApBase *				m_pcsBase;
	ApdEventAttachData *	m_pstEventAD;
	ApmEventManager *		m_pcsApmEventManager;

// Construction
public:
	void InitData(ApBase *pcsBase, ApdEventAttachData *pstEventAD, ApmEventManager *pcsApmEventManager);
	ApmEventListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ApmEventListDlg)
	enum { IDD = IDD_EVENT_LIST };
	CComboBox	m_csEventSelect;
	CListBox	m_csEventList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ApmEventListDlg)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ApmEventListDlg)
	afx_msg void OnEventAdd();
	afx_msg void OnEventRemove();
	afx_msg void OnEventEdit();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APMEVENTLISTDLG_H__3B9C39E7_F7B8_45C8_9E6B_ADA21B07085E__INCLUDED_)
