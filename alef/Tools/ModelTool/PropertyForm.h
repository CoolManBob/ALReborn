#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "PropertyTree.h"

class CPropertyForm : public CFormView
{
protected:
	CPropertyForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPropertyForm)

public:
	CPropertyForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor = NULL);


protected:
	VOID InitializeMember();
	BOOL CreateMember();

protected:
	HBRUSH			m_hBackBrush;
	CPropertyTree*	m_pcsPropertyTree;

// Form Data
public:
	//{{AFX_DATA(CPropertyForm)
	enum { IDD = IDD_PROPERTY_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyForm();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPropertyForm)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};