#if !defined(AFX_EVENTPRODUCT_H__26B654B0_0155_477E_9303_F467C87C28A2__INCLUDED_)
#define AFX_EVENTPRODUCT_H__26B654B0_0155_477E_9303_F467C87C28A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventProduct.h : header file
//

#include "../resource.h"		// main symbols

#define EVENT_PRODUCT_CATEGORY_MAX		3

/////////////////////////////////////////////////////////////////////////////
// CEventProduct dialog
class AgpmProduct;
class CEventProduct : public CDialog
{
public:
	static INT32 s_lCategory[EVENT_PRODUCT_CATEGORY_MAX];

	AgpmProduct	*m_pAgpmProduct;
	ApdEvent	*m_pApdEvent;	
// Construction
public:
	CEventProduct(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventProduct)
	enum { IDD = IDD_EVENT_PRODUCT_DLG };
	CComboBox	m_Combo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventProduct)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventProduct)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTPRODUCT_H__26B654B0_0155_477E_9303_F467C87C28A2__INCLUDED_)
