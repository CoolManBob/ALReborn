#if !defined(AFX_FRMVIEW_H__C31E5D19_5FD5_42ED_9411_CDA0BD2CE0F7__INCLUDED_)
#define AFX_FRMVIEW_H__C31E5D19_5FD5_42ED_9411_CDA0BD2CE0F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrmView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CFrmView : public CFormView
{
protected:
	CFrmView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFrmView)

// Form Data
public:
	//{{AFX_DATA(CFrmView)
	enum { IDD = IDD_DLG_FORMVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFrmView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFrmView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMVIEW_H__C31E5D19_5FD5_42ED_9411_CDA0BD2CE0F7__INCLUDED_)
