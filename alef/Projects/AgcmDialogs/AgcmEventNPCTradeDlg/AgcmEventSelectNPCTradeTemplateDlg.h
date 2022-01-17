#if !defined(AFX_AGCMEVENTSELECTNPCTRADETEMPLATEDLG_H__507707E9_DA1E_40D4_B1D3_2BD07893251B__INCLUDED_)
#define AFX_AGCMEVENTSELECTNPCTRADETEMPLATEDLG_H__507707E9_DA1E_40D4_B1D3_2BD07893251B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmEventSelectNPCTradeTemplateDlg.h : header file
//

#include "../Resource.h"
#include "AgpmEventNPCTrade.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSelectNPCTradeTemplateDlg dialog

class AgcmEventSelectNPCTradeTemplateDlg : public CDialog
{
// Construction
	AgpdEventNPCTradeData	*m_pcsNPCDataData;

	ApmObject				*m_pcsApmObject;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmEventNPCTrade		*m_pcsAgpmEventNPCTrade;
	ApAdmin					m_csNPCTradeTemplate;

public:
	void InitData( ApmObject *pcsApmObject, AgpmCharacter *pcsAgpmCharacter, AgpmEventNPCTrade *pcsAgpmEventNPCTrade, AgpdEventNPCTradeData *pcsEventData );

	AgcmEventSelectNPCTradeTemplateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmEventSelectNPCTradeTemplateDlg)
	enum { IDD = IDD_NPCTradeTemplateSelectDlg };
	CComboBox	m_cNPCTradeTemplateCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmEventSelectNPCTradeTemplateDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmEventSelectNPCTradeTemplateDlg)
	afx_msg void OnNPCTradeTemplateSelectButton();
	afx_msg void OnNPCTradeTempalteDeleteButton();
	afx_msg void OnNPCTradeTemplateOKButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnNPCTradeTemplateCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTSELECTNPCTRADETEMPLATEDLG_H__507707E9_DA1E_40D4_B1D3_2BD07893251B__INCLUDED_)
