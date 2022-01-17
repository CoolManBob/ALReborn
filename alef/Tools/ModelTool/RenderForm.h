#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "RenderWare.h"
#include "RenderView.h"

class CRenderForm : public CFormView
{
protected:
	CRenderForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRenderForm)

public:
	CRenderForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor = NULL);

protected:
	HBRUSH			m_hBackBrush;

	CRenderView		*m_pcsRenderView;
	CRenderWare		*m_pcsRenderWare;

public:
	BOOL			UpdateRenderForm();

	CRenderView		*GetRenderView()		{return m_pcsRenderView;}
	CRenderWare		*GetRenderWare()		{return m_pcsRenderWare;}

protected:
	VOID			InitializeMember();
	BOOL			CreateMember();


// Form Data
public:
	//{{AFX_DATA(CRenderForm)
	enum { IDD = IDD_RENDER_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderForm();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CRenderForm)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};