#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CMenuForm : public CFormView
{
protected:
	CMenuForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMenuForm)

public:
	CMenuForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor = NULL);

protected:
	HBRUSH			m_hBackBrush;
	CToolBarCtrl	*m_pcsMenuToolBar;
	CBitmap			*m_pcsMenuToolBarBitmap;
	CToolTipCtrl	*m_pcsMenuToolTip;

protected:
	VOID InitializeMember();
	BOOL CreateMember();

// Form Data
public:
	//{{AFX_DATA(CMenuForm)
	enum { IDD = IDD_MENU_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMenuForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMenuForm();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMenuForm)
	afx_msg void OnDestroy();
	afx_msg void OnCommandPressSaveButton();
	afx_msg void OnCommandPressColorButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};