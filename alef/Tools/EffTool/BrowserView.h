#if !defined(AFX_BROWSERVIEW_H__D8D29BF8_BFD2_4265_ADAC_7687DC0705AD__INCLUDED_)
#define AFX_BROWSERVIEW_H__D8D29BF8_BFD2_4265_ADAC_7687DC0705AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrowserView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrowserView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

class CBrowserView : public CHtmlView
{
protected:
	CBrowserView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrowserView)

// html Data
public:
	//{{AFX_DATA(CBrowserView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBrowserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBrowserView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSERVIEW_H__D8D29BF8_BFD2_4265_ADAC_7687DC0705AD__INCLUDED_)
