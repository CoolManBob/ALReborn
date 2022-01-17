#if !defined(AFX_AGCMEVENTAUCTIONDLGDIALOG_H__978DA20F_B68A_4CDE_BA25_E8F78EF75052__INCLUDED_)
#define AFX_AGCMEVENTAUCTIONDLGDIALOG_H__978DA20F_B68A_4CDE_BA25_E8F78EF75052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmEventAuctionDlgDialog.h : header file
//

#include "../Resource.h"
#include "AgpmAuction.h"

/////////////////////////////////////////////////////////////////////////////
// CAgcmEventAuctionDlgDialog dialog

class CAgcmEventAuctionDlgDialog : public CDialog
{
// Construction
	AgpdEventAuction		*m_pcsEventData;

	ApmObject				*m_pcsApmObject;
	AgpmCharacter			*m_pcsAgpmCharacter;

public:
	CAgcmEventAuctionDlgDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAgcmEventAuctionDlgDialog)
	enum { IDD = IDD_EventAuctionDialog };
	CStatic	m_cEventAuctionStatic;
	//}}AFX_DATA

	void InitData( ApmObject *pcsApmObject, AgpmCharacter *pcsAgpmCharacter, AgpdEventAuction *pcsEventData );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgcmEventAuctionDlgDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAgcmEventAuctionDlgDialog)
	afx_msg void OnEventAuctionCancelButton();
	afx_msg void OnEventAuctionApplyButton();
	afx_msg void OnEventAuctionExitButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTAUCTIONDLGDIALOG_H__978DA20F_B68A_4CDE_BA25_E8F78EF75052__INCLUDED_)
