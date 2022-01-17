#if !defined(AFX_AGCMAIUSEITEMEDIT_H__FFA14E8D_2891_4D4E_84EE_47AC7AD6C93B__INCLUDED_)
#define AFX_AGCMAIUSEITEMEDIT_H__FFA14E8D_2891_4D4E_84EE_47AC7AD6C93B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAIUseItemEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmAIUseItemEdit dialog

#include "AgpmAI.h"
#include "AgpmItem.h"

class AgcmAIUseItemEdit : public CDialog
{
// Construction
public:
	AgcmAIUseItemEdit(CWnd* pParent = NULL);   // standard constructor

	AgpmItem		*m_pcsAgpmItem;
	AgpdAITemplate	*m_pstTemplate;

public:
	AddItemToComboBox( char *pstrItemName, long lTID );
// Dialog Data
	//{{AFX_DATA(AgcmAIUseItemEdit)
	enum { IDD = IDD_DIALOG_AI_EDIT_USE_ITEM };
	CEdit	m_cHPConEdit;
	CEdit	m_cRateEdit;
	CListCtrl	m_ItemList;
	CComboBox	m_cItemCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAIUseItemEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAIUseItemEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnAiUseItemOkButton();
	afx_msg void OnAiUseItemCancelButton();
	afx_msg void OnAiUseItemAddButton();
	afx_msg void OnAiUseItemRemoveButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAIUSEITEMEDIT_H__FFA14E8D_2891_4D4E_84EE_47AC7AD6C93B__INCLUDED_)
