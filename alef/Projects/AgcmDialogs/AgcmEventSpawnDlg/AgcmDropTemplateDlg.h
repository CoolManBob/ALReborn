#if !defined(AFX_AGCMDROPTEMPLATEDLG_H__D0C0FD0E_E91E_4AA1_AAB8_7992D4FA92D8__INCLUDED_)
#define AFX_AGCMDROPTEMPLATEDLG_H__D0C0FD0E_E91E_4AA1_AAB8_7992D4FA92D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmDropTemplateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmDropTemplateDlg dialog

#include "../Resource.h"
#include "AgpmItem.h"
#include "AgpmDropItem.h"

class AgcmDropTemplateDlg : public CDialog
{
	AgpdDropItemTemplate		*m_pcsAgpdDropItemTemplate;
	AgpmItem					*m_pcsAgpmItem;
	AgpmDropItem				*m_pcsAgpmDropItem;

// Construction
public:
	void ShowCategory( eAgpmDropItemCategory eCategory );

	void SetDlgData( AgpmItem *pcsAgpmItem, AgpmDropItem *pcsAgpmDropItem, AgpdDropItemTemplate *pcsAgpdDropItemTemplate );
	void SetDropItemTemplate( AgpdDropItemTemplate *pcsAgpdDropItemTemplate );
	void CleanDropItemListRow( int iRow );

	void SetDropDataFromDropTemplate( eAgpmDropItemCategory eCategory, int lRow );

	AgcmDropTemplateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmDropTemplateDlg)
	enum { IDD = IDD_DIALOG_DROP_TEMPLATE_DLG };
	CEdit	m_cDropRateEdit;
	CEdit	m_cRelasteValueEdit;
	CListCtrl	m_cDropInfoList;
	CStatic	m_cDropInfoStaic;
	CComboBox	m_cDropTemplateCombo;
	CComboBox	m_cDropItemListCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmDropTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmDropTemplateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboDropCategory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMDROPTEMPLATEDLG_H__D0C0FD0E_E91E_4AA1_AAB8_7992D4FA92D8__INCLUDED_)
