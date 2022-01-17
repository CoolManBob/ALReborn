#if !defined(AFX_AGCMEVENTBINDINGPOINT_H__3424ADE8_C126_4902_BD84_7D2C7207DE74__INCLUDED_)
#define AFX_AGCMEVENTBINDINGPOINT_H__3424ADE8_C126_4902_BD84_7D2C7207DE74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmEventBindingPoint.h : header file
//

#include "../Resource.h"

#include "ApmEventManager.h"
#include "AgpmEventBinding.h"
#include "AgpmFactors.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventBindingPoint dialog

class AgcmEventBindingPoint : public CDialog
{
private:
	ApmEventManager			*m_pcsApmEventManager;
	AgpmEventBinding		*m_pcsAgpmEventBinding;
	AgpmFactors				*m_pcsAgpmFactors;
	ApmMap					*m_pcsApmMap;

	ApdEvent				*m_pcsEvent;

	CHAR					m_astrBindingTypeName[AGPDBINDING_TYPE_MAX][AGPDBINDING_MAX_NAME + 1];
	CHAR					m_astrBindingCharacterTypeName[AURACE_TYPE_MAX * AUCHARCLASS_TYPE_MAX][64];

// Construction
public:
	AgcmEventBindingPoint(CWnd* pParent = NULL);
	AgcmEventBindingPoint(ApmEventManager *pcsApmEventManager,
						  AgpmEventBinding *pcsAgpmEventBinding,
						  AgpmFactors *pcsAgpmFactors,
						  ApmMap *pcsApmMap,
						  ApdEvent *pcsEvent,
						  CWnd* pParent = NULL);   // standard constructor

	BOOL					InitEventData();

// Dialog Data
	//{{AFX_DATA(AgcmEventBindingPoint)
	enum { IDD = IDD_DIALOG_BINDING };
	CComboBox	m_csCharacterType;
	CComboBox	m_csBindingType;
	CComboBox	m_csRegionName;
	CString	m_strBindingName;
	UINT	m_ulRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmEventBindingPoint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmEventBindingPoint)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTBINDINGPOINT_H__3424ADE8_C126_4902_BD84_7D2C7207DE74__INCLUDED_)
