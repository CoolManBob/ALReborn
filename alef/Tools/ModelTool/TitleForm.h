#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "EventButton.h"

class CTitleForm : public CFormView
{
protected:
	CTitleForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTitleForm)

protected:
	VOID InitializeMember();
	VOID CreateMember();

public:
	static CTitleForm*	m_pThisTitleForm;

	CTitleForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor = NULL);
	VOID SetCurrentTitle(CHAR *szTitle);

protected:
	HBRUSH			m_hBackBrush;

	CRect			m_rtCurTitle;
	CEdit			*m_pcsCurTitle;

// Form Data
public:
	//{{AFX_DATA(CTitleForm)
	enum { IDD = IDD_TITLE_FORM };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitleForm();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTitleForm)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};